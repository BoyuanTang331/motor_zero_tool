#include "mainwindow.hpp"

#include <QApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include <fstream>
#include <sstream>
#include <thread>

namespace taihu_zero {

// ---------------------------------------------------------------------------
// ToolConfig 解析：key: value 行格式（零外部依赖）
// ---------------------------------------------------------------------------
ToolConfig loadToolConfig(const std::string& path, std::string* error) {
  ToolConfig cfg;
  std::ifstream in(path);
  if (!in.good()) {
    if (error) *error = "无法打开配置文件: " + path;
    return cfg;
  }
  std::string line;
  std::size_t line_no = 0;
  while (std::getline(in, line)) {
    ++line_no;
    const auto hash = line.find('#');
    if (hash != std::string::npos) line = line.substr(0, hash);
    const auto colon = line.find(':');
    if (colon == std::string::npos) continue;
    auto trim = [](std::string s) {
      const auto a = s.find_first_not_of(" \t\r\n");
      const auto b = s.find_last_not_of(" \t\r\n");
      return a == std::string::npos ? std::string{} : s.substr(a, b - a + 1);
    };
    const std::string key = trim(line.substr(0, colon));
    const std::string value = trim(line.substr(colon + 1));
    if (value.empty()) continue;
    try {
      if (key == "eni_file") cfg.eni_file = value;
      else if (key == "driver_yaml_path") cfg.driver_yaml_path = value;
      else if (key == "receipt_dir") cfg.receipt_dir = value;
      else if (key == "master_id") cfg.master_id = std::stoi(value);
      else if (key == "cpu_affinity") cfg.cpu_affinity = std::stoi(value);
      else if (key == "priority") cfg.priority = std::stoi(value);
      else if (key == "cycle_time_ns") cfg.cycle_time_ns = std::stoll(value);
      else if (key == "encoder_resolution") cfg.encoder_resolution = std::stod(value);
      else if (key == "motor_dir") {
        std::istringstream iss(value);
        std::string token;
        std::size_t i = 0;
        while (std::getline(iss, token, ',') && i < 4) {
          cfg.motor_dir[i++] = std::stoi(token);
        }
      }
    } catch (...) {
      if (error) {
        *error = "配置解析失败: 第 " + std::to_string(line_no) + " 行 key=" + key;
      }
    }
  }
  return cfg;
}

// ---------------------------------------------------------------------------
// ZeroWorker
// ---------------------------------------------------------------------------
ZeroWorker::ZeroWorker(ZeroMaster* master, QObject* parent)
    : QObject(parent), master_(master) {
  options_.transition_timeout = std::chrono::seconds(2);
  options_.stable_frame_count = 3;
  options_.software_capture_samples = 50;
  options_.software_capture_interval = std::chrono::milliseconds(20);
  options_.cancellation_requested = [this]() { return cancel_.load(); };
}

void ZeroWorker::configure(std::size_t axis_count,
                           const ZeroingOptions& options,
                           double encoder_resolution,
                           const std::array<int, 4>& motor_dir) {
  axis_count_ = axis_count;
  options_.transition_timeout = options.transition_timeout;
  options_.stable_frame_count = options.stable_frame_count;
  options_.software_capture_samples = options.software_capture_samples;
  options_.software_capture_interval = options.software_capture_interval;
  encoder_resolution_ = encoder_resolution;
  motor_dir_ = motor_dir;
}

void ZeroWorker::requestCancel() { cancel_.store(true); }

// exchangeOnce — 对应 Ti5 的 exchange lambda：
// stage 输出 -> 等至少一个周期帧 -> 读快照
ZeroObservation ZeroWorker::exchangeOnce(const ZeroCommand& command) {
  master_->stage_command(command.axis, command.control_word,
                         command.operation_mode, command.target_position,
                         command.target_position_valid);
  const std::uint64_t start_frame = master_->frame_counter();
  const auto deadline =
      std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
  while (master_->frame_counter() == start_frame) {
    if (std::chrono::steady_clock::now() > deadline) {
      return ZeroObservation{};  // 超时 = 交换失败
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  const auto snap = master_->snapshot(command.axis);
  ZeroObservation obs;
  obs.exchange_succeeded = snap.pdo_valid;
  obs.status_word = snap.status_word;
  obs.error_code = snap.error_code;
  obs.actual_position = snap.position_counts;
  obs.actual_velocity = snap.velocity_counts;
  obs.operation_mode_display = snap.mode_display;
  return obs;
}

void ZeroWorker::probeHm35() {
  emit logLine("[探测] 逐轴 SDO 读 0x6098:00（homing method 对象）...");
  ZeroSdoOps sdo;
  sdo.read_i8 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                       std::int8_t* v) {
    std::string err;
    return master_->sdo_read<std::int8_t>(pos, idx, sub, v, &err);
  };
  sdo.read_i32 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                        std::int32_t* v) {
    std::string err;
    return master_->sdo_read<std::int32_t>(pos, idx, sub, v, &err);
  };
  sdo.write_i8 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                        std::int8_t v) {
    std::string err;
    return master_->sdo_write<std::int8_t>(pos, idx, sub, v, &err);
  };
  int ok_count = 0;
  for (std::size_t i = 0; i < axis_count_; ++i) {
    const auto slave_pos =
        static_cast<std::uint16_t>(master_->snapshot(i).slave_pos);
    std::int8_t method = 0;
    if (probeHm35Support(sdo, slave_pos, &method)) {
      emit logLine(QString("[探测] 轴 %1 (slave %2): 0x6098 可读, 当前方法=%3")
                       .arg(i)
                       .arg(slave_pos)
                       .arg(static_cast<int>(method)));
      ++ok_count;
    } else {
      emit logLine(QString("[探测] 轴 %1 (slave %2): 0x6098 不可读 → 固件不支持 HM")
                       .arg(i)
                       .arg(slave_pos));
    }
  }
  const bool all = ok_count == static_cast<int>(axis_count_);
  emit hm35ProbeDone(all, all ? "4 轴均支持 0x6098, 可用 HM35 硬件刷零"
                              : "存在不支持轴, 请使用软件零位");
}

void ZeroWorker::runSoftwareZero() {
  cancel_.store(false);
  emit logLine("[软件零位] 开始逐轴采集（SDO 读 0x6064 × 50 取中值）...");
  ZeroSdoOps sdo;
  sdo.read_i32 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                        std::int32_t* v) {
    std::string err;
    return master_->sdo_read<std::int32_t>(pos, idx, sub, v, &err);
  };
  sdo.read_i8 = nullptr;
  sdo.write_i8 = nullptr;
  ZeroPdoExchange exchange = [this](const ZeroCommand& c) { return exchangeOnce(c); };

  QStringList offsets, previews;
  bool all_ok = true;
  for (std::size_t i = 0; i < axis_count_; ++i) {
    const auto slave_pos =
        static_cast<std::uint16_t>(master_->snapshot(i).slave_pos);
    emit progress(static_cast<int>(i * 100 / axis_count_));
    const auto capture = runSoftwareZeroCapture(slave_pos, sdo, exchange, options_);
    if (!capture.ok) {
      emit logLine(QString("[软件零位] 轴 %1 (slave %2) 采集失败: %3")
                       .arg(i)
                       .arg(slave_pos)
                       .arg(QString::fromStdString(capture.error)));
      all_ok = false;
      break;
    }
    pending_offsets_[i] = capture.offset_counts;
    const double q = countsToRadians(capture.median_counts,
                                     capture.offset_counts, motor_dir_[i],
                                     encoder_resolution_);
    emit logLine(QString("[软件零位] 轴 %1 (slave %2): offset=%3 (q=%4 rad)")
                     .arg(i)
                     .arg(slave_pos)
                     .arg(capture.offset_counts)
                     .arg(q, 0, 'f', 5));
    offsets << QString::number(capture.offset_counts);
    previews << QString("轴%1: %2 counts → q=%3 rad")
                    .arg(i)
                    .arg(capture.offset_counts)
                    .arg(q, 0, 'f', 5);
  }
  emit progress(100);
  if (all_ok && offsets.size() == 4) {
    emit softwareOffsetsReady(offsets, previews);
    emit softwareDone(true, "4 轴软件零位采集完成, 待写入 YAML");
  } else {
    emit softwareDone(false, "软件零位采集失败, 见日志");
  }
}

void ZeroWorker::runHm35Zero() {
  cancel_.store(false);
  emit logLine("[HM35] 开始硬件刷零流程（7 步控制字时序, 仿 Ti5 装配调零）...");
  ZeroSdoOps sdo;
  sdo.read_i8 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                       std::int8_t* v) {
    std::string err;
    return master_->sdo_read<std::int8_t>(pos, idx, sub, v, &err);
  };
  sdo.read_i32 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                        std::int32_t* v) {
    std::string err;
    return master_->sdo_read<std::int32_t>(pos, idx, sub, v, &err);
  };
  sdo.write_i8 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                        std::int8_t v) {
    std::string err;
    return master_->sdo_write<std::int8_t>(pos, idx, sub, v, &err);
  };
  sdo.write_u16 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                         std::uint16_t v) {
    std::string err;
    return master_->sdo_write<std::uint16_t>(pos, idx, sub, v, &err);
  };
  sdo.write_u32 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                         std::uint32_t v) {
    std::string err;
    return master_->sdo_write<std::uint32_t>(pos, idx, sub, v, &err);
  };
  sdo.read_u16 = [this](std::uint16_t pos, std::uint16_t idx, std::uint8_t sub,
                        std::uint16_t* v) {
    std::string err;
    return master_->sdo_read<std::uint16_t>(pos, idx, sub, v, &err);
  };
  ZeroPdoExchange exchange = [this](const ZeroCommand& c) { return exchangeOnce(c); };

  std::vector<std::pair<std::size_t, std::uint16_t>> targets;
  for (std::size_t i = 0; i < axis_count_; ++i) {
    targets.emplace_back(i, static_cast<std::uint16_t>(
                                master_->snapshot(i).slave_pos));
  }
  last_hm35_result_ = runHm35Zeroing(targets, sdo, exchange, options_);

  for (const auto& e : last_hm35_result_.entries) {
    emit logLine(QString("[HM35] 轴 %1: before=%2 after=%3 zero_ok=%4 csp_ok=%5")
                     .arg(e.axis)
                     .arg(e.position_before.has_value()
                              ? QString::number(*e.position_before)
                              : "n/a")
                     .arg(e.position_after.has_value()
                              ? QString::number(*e.position_after)
                              : "n/a")
                     .arg(e.zero_verified ? 1 : 0)
                     .arg(e.disabled_and_csp_restored ? 1 : 0));
  }
  for (const auto& issue : last_hm35_result_.issues) {
    QString line = QString("[HM35][错误] %1 axis=%2 detail=%3")
                       .arg(QString::fromStdString(zeroIssueCodeName(issue.code)))
                       .arg(issue.axis.has_value() ? QString::number(*issue.axis) : "-")
                       .arg(QString::fromStdString(issue.detail));
    if (issue.transition.has_value()) {
      line += " " + QString::fromStdString(
                        formatZeroTransitionDiagnostics(*issue.transition));
    }
    emit logLine(line);
  }
  if (!last_hm35_result_.ok()) {
    emit hm35Done(false, "HM35 刷零失败, 已执行失败遏制, 见日志");
    return;
  }

  // HM35 成功后必须写 Flash 才掉电不丢（仿 merman saveTi5Parameters）
  emit logLine("[HM35] 刷零完成, 开始 Flash 参数保存（0x1010 'save' / 0x2000 探测）...");
  bool flash_all_ok = true;
  for (const auto& [axis, slave_pos] : targets) {
    const auto save = runFlashSave(slave_pos, sdo);
    if (save.ok) {
      emit logLine(QString("[Flash] 轴 %1 (slave %2): 保存成功 (方法=%3)%4")
                       .arg(axis)
                       .arg(slave_pos)
                       .arg(save.method == FlashSaveMethod::Standard1010
                                ? "0x1010"
                                : "0x2000")
                       .arg(save.error.empty()
                                ? ""
                                : " 备注: " + QString::fromStdString(save.error)));
    } else {
      flash_all_ok = false;
      emit logLine(QString("[Flash][警告] 轴 %1 (slave %2): %3")
                       .arg(axis)
                       .arg(slave_pos)
                       .arg(QString::fromStdString(save.error)));
    }
  }
  if (flash_all_ok) {
    emit hm35Done(true,
        "4 轴 HM35 刷零完成且已写入 Flash（掉电不丢）。"
        "驱动侧 motor_offset 应归 [0,0,0,0]");
  } else {
    emit hm35Done(true,
        "4 轴 HM35 刷零完成, 但部分轴 Flash 保存未确认——"
        "零点可能掉电丢失, 见日志 [Flash] 行");
  }
}

void ZeroWorker::readEncoders() {
  emit logLine("[读编码盘] SDO 读 4 轴 0x6064（原始编码器 counts）...");
  QStringList lines;
  for (std::size_t i = 0; i < axis_count_; ++i) {
    const auto snap = master_->snapshot(i);
    std::string err;
    std::int32_t counts = 0;
    const bool ok = master_->sdo_read<std::int32_t>(
        snap.slave_pos, 0x6064, 0, &counts, &err);
    if (ok) {
      const double q = countsToRadians(counts, pending_offsets_[i],
                                       motor_dir_[i], encoder_resolution_);
      const auto line = QString("轴 %1 (slave %2): %3 counts  (q=%4 rad)")
                            .arg(i)
                            .arg(snap.slave_pos)
                            .arg(counts)
                            .arg(q, 0, 'f', 5);
      emit logLine("  " + line);
      lines << line;
    } else {
      const auto line = QString("轴 %1 (slave %2): 读取失败: %3")
                            .arg(i)
                            .arg(snap.slave_pos)
                            .arg(QString::fromStdString(err));
      emit logLine("  " + line);
      lines << line;
    }
  }
  emit encoderReadDone(lines);
}

void ZeroWorker::writeOffsetsToYaml(const QString& yaml_path) {
  emit logLine("[写回] 写 motor_offset → " + yaml_path);
  const auto result = writeMotorOffsets(yaml_path.toStdString(), pending_offsets_);
  if (!result.ok) {
    emit yamlWritten(false, yaml_path, "");
    return;
  }
  // 写后回读校验
  const auto verify = readMotorOffsets(yaml_path.toStdString());
  const bool verified = verify.ok && verify.offsets == pending_offsets_;
  emit yamlWritten(verified, yaml_path,
                   QString::fromStdString(result.backup_path));
}

// ---------------------------------------------------------------------------
// MainWindow
// ---------------------------------------------------------------------------
MainWindow::MainWindow(const ToolConfig& config, QWidget* parent)
    : QMainWindow(parent), config_(config) {
  buildUi();

  worker_ = new ZeroWorker(&master_);
  worker_->moveToThread(&worker_thread_);
  connect(&worker_thread_, &QThread::finished, worker_, &QObject::deleteLater);
  connect(worker_, &ZeroWorker::logLine, this, &MainWindow::appendLog);
  connect(worker_, &ZeroWorker::softwareOffsetsReady, this,
          &MainWindow::onSoftwareOffsets);
  connect(worker_, &ZeroWorker::hm35ProbeDone, this, &MainWindow::onHm35Probe);
  connect(worker_, &ZeroWorker::hm35Done, this, &MainWindow::onHm35Done);
  connect(worker_, &ZeroWorker::softwareDone, this, &MainWindow::onSoftwareDone);
  connect(worker_, &ZeroWorker::encoderReadDone, this, &MainWindow::onEncoderRead);
  connect(worker_, &ZeroWorker::yamlWritten, this, &MainWindow::onYamlWritten);
  worker_->configure(kExpectedAxes, ZeroingOptions{}, config_.encoder_resolution,
                     config_.motor_dir);
  worker_thread_.start();

  refresh_timer_.setInterval(100);
  connect(&refresh_timer_, &QTimer::timeout, this, &MainWindow::refreshSnapshots);
  refresh_timer_.start();
  setWorkflowEnabled(false);
  appendLog("未连接主站。确认: 1) 已停止 taihu_steer_driver; 2) ethercatctl start 已运行。");
}

MainWindow::~MainWindow() {
  worker_thread_.quit();
  worker_thread_.wait(3000);
}

void MainWindow::buildUi() {
  auto* central = new QWidget(this);
  auto* root = new QVBoxLayout(central);

  auto* title = new QLabel("Taihu 底盘转向电机标零工具（IgH 工控机版）", this);
  title->setStyleSheet("font-size:16px;font-weight:bold;");
  root->addWidget(title);

  // 状态栏
  auto* status_row = new QHBoxLayout();
  status_label_ = new QLabel("状态: 未连接", this);
  status_label_->setStyleSheet("font-weight:bold;");
  status_row->addWidget(status_label_);
  status_row->addStretch();
  status_row->addWidget(new QLabel("操作员:", this));
  operator_edit_ = new QLineEdit(this);
  operator_edit_->setPlaceholderText("姓名/工号");
  operator_edit_->setMaximumWidth(140);
  status_row->addWidget(operator_edit_);
  root->addLayout(status_row);

  // 4 轴卡片
  auto* cards = new QGroupBox("转向电机实时状态（100ms 刷新）", this);
  auto* cards_layout = new QHBoxLayout(cards);
  for (std::size_t i = 0; i < 4; ++i) {
    axis_labels_[i] = new QLabel(QString("轴 %1\n未连接").arg(i), this);
    axis_labels_[i]->setMinimumWidth(160);
    axis_labels_[i]->setStyleSheet(
        "border:1px solid #999;border-radius:6px;padding:8px;background:#f7f7f7;");
    cards_layout->addWidget(axis_labels_[i]);
  }
  root->addWidget(cards);

  // 操作按钮
  auto* ops = new QGroupBox("标零流程（按顺序执行）", this);
  auto* ops_layout = new QHBoxLayout(ops);
  start_btn_ = new QPushButton("1. 启动主站", this);
  stop_btn_ = new QPushButton("停止主站", this);
  read_btn_ = new QPushButton("一键读编码盘值", this);
  probe_btn_ = new QPushButton("2. HM35 探测", this);
  hm35_btn_ = new QPushButton("3. HM35 硬件刷零(推荐)", this);
  software_btn_ = new QPushButton("4. 软件零位(降级备选)", this);
  write_btn_ = new QPushButton("5. 写入 YAML+回执", this);
  ops_layout->addWidget(start_btn_);
  ops_layout->addWidget(stop_btn_);
  ops_layout->addWidget(read_btn_);
  ops_layout->addWidget(probe_btn_);
  ops_layout->addWidget(hm35_btn_);
  ops_layout->addWidget(software_btn_);
  ops_layout->addWidget(write_btn_);
  root->addWidget(ops);

  log_ = new QTextEdit(this);
  log_->setReadOnly(true);
  log_->setStyleSheet("font-family:monospace;font-size:11px;");
  root->addWidget(log_);

  connect(start_btn_, &QPushButton::clicked, this, &MainWindow::onStartMaster);
  connect(stop_btn_, &QPushButton::clicked, this, &MainWindow::onStopMaster);
  connect(read_btn_, &QPushButton::clicked, this, &MainWindow::onReadEncoders);
  connect(probe_btn_, &QPushButton::clicked, this, &MainWindow::onProbeHm35);
  connect(software_btn_, &QPushButton::clicked, this, &MainWindow::onSoftwareZero);
  connect(hm35_btn_, &QPushButton::clicked, this, &MainWindow::onHm35Zero);
  connect(write_btn_, &QPushButton::clicked, this, &MainWindow::onWriteYaml);

  setCentralWidget(central);
  resize(1100, 720);
}

void MainWindow::setWorkflowEnabled(bool master_running) {
  start_btn_->setEnabled(!master_running);
  stop_btn_->setEnabled(master_running);
  read_btn_->setEnabled(master_running);
  probe_btn_->setEnabled(master_running);
  software_btn_->setEnabled(master_running);
  hm35_btn_->setEnabled(master_running && hm35_supported_);
  write_btn_->setEnabled(offsets_pending_);
}

void MainWindow::onStartMaster() {
  MasterOptions opts;
  opts.master_id = config_.master_id;
  opts.cpu_affinity = config_.cpu_affinity;
  opts.priority = config_.priority;
  opts.cycle_time_ns = config_.cycle_time_ns;
  opts.eni_file = config_.eni_file;
  std::string error;
  appendLog("[主站] 启动 EtherCAT（ENI: " +
            QString::fromStdString(opts.eni_file) + "）...");
  if (!master_.start(opts, &error)) {
    appendLog("[主站][失败] " + QString::fromStdString(error));
    QMessageBox::critical(this, "主站启动失败", QString::fromStdString(error));
    return;
  }
  master_running_ = true;
  status_label_->setText(QString("状态: 已连接, %1 轴 OP").arg(master_.axis_count()));
  appendLog(QString("[主站] 已启动, 映射 %1 个 CiA402 轴").arg(master_.axis_count()));
  setWorkflowEnabled(true);
}

void MainWindow::onStopMaster() {
  master_.stop();
  master_running_ = false;
  status_label_->setText("状态: 未连接");
  appendLog("[主站] 已停止（全部轴已恢复失能 CSP）");
  setWorkflowEnabled(false);
}

void MainWindow::onProbeHm35() {
  QMetaObject::invokeMethod(worker_, "probeHm35", Qt::QueuedConnection);
}

void MainWindow::onReadEncoders() {
  QMetaObject::invokeMethod(worker_, "readEncoders", Qt::QueuedConnection);
}

void MainWindow::onSoftwareZero() {
  offsets_pending_ = false;
  setWorkflowEnabled(master_running_);
  QMetaObject::invokeMethod(worker_, "runSoftwareZero", Qt::QueuedConnection);
}

void MainWindow::onHm35Zero() {
  const auto answer = QMessageBox::warning(
      this, "HM35 硬件刷零",
      "即将对 4 轴执行 HM35（当前位置设为编码器零点）。\n"
      "请确认: 1) 4 个转向轮已摆到机械零位; 2) 电机静止无故障。\n继续？",
      QMessageBox::Yes | QMessageBox::No);
  if (answer != QMessageBox::Yes) return;
  QMetaObject::invokeMethod(worker_, "runHm35Zero", Qt::QueuedConnection);
}

void MainWindow::onWriteYaml() {
  QMetaObject::invokeMethod(worker_, "writeOffsetsToYaml",
                            Qt::QueuedConnection,
                            Q_ARG(QString,
                                  QString::fromStdString(config_.driver_yaml_path)));
}

void MainWindow::refreshSnapshots() {
  if (!master_running_) return;
  for (std::size_t i = 0; i < 4; ++i) axis_labels_[i]->setText(axisCardText(i));
}

QString MainWindow::axisCardText(std::size_t axis) const {
  const auto s = master_.snapshot(axis);
  // 采集/写入后始终用 pending_offsets_ 显示 q（此时 q 应≈0，直观验证）
  const double q = countsToRadians(s.position_counts, pending_offsets_[axis],
                                   config_.motor_dir[axis],
                                   config_.encoder_resolution);
  return QString("轴 %1 (slave %2)\npos: %3\nvel: %4\nq: %5 rad\nstatus: 0x%6\nmode: %7\nerr: 0x%8")
      .arg(axis)
      .arg(s.slave_pos)
      .arg(s.position_counts)
      .arg(s.velocity_counts)
      .arg(q, 0, 'f', 4)
      .arg(s.status_word, 4, 16, QLatin1Char('0'))
      .arg(static_cast<int>(s.mode_display))
      .arg(s.error_code, 4, 16, QLatin1Char('0'));
}

void MainWindow::appendLog(const QString& text) {
  log_->append(text);
}

void MainWindow::onSoftwareOffsets(const QStringList& offsets,
                                   const QStringList& previews) {
  for (int i = 0; i < 4 && i < offsets.size(); ++i) {
    pending_offsets_[i] = offsets[i].toInt();
  }
  offsets_pending_ = true;
  setWorkflowEnabled(master_running_);
  appendLog("[软件零位] offset 预览: [" + offsets.join(", ") + "]");
  for (const auto& p : previews) appendLog("  " + p);
}

void MainWindow::onSoftwareDone(bool ok, const QString& summary) {
  appendLog(ok ? "[软件零位] " + summary : "[软件零位][失败] " + summary);
}

void MainWindow::onHm35Probe(bool supported, const QString& detail) {
  hm35_supported_ = supported;
  setWorkflowEnabled(master_running_);
  appendLog("[探测] " + detail);
}

void MainWindow::onEncoderRead(const QStringList& lines) {
  appendLog("[读编码盘] 共 " + QString::number(lines.size()) + " 条记录, 见上");
}

void MainWindow::onHm35Done(bool ok, const QString& summary) {
  appendLog(ok ? "[HM35] " + summary : "[HM35][失败] " + summary);
  if (ok) {
    QMessageBox::information(this, "HM35 完成", summary +
        "\n请重启 taihu_steer_driver 验证（其 motor_offset 应为 [0,0,0,0]）。");
  }
}

void MainWindow::onYamlWritten(bool ok, const QString& path, const QString& backup) {
  if (!ok) {
    appendLog("[写回][失败] " + path);
    QMessageBox::critical(this, "写回失败", "motor_offset 写入失败: " + path);
    return;
  }
  appendLog("[写回] 成功: " + path + "（备份: " + backup + "）");
  // 回执
  ReceiptContext ctx;
  ctx.operation_id = makeOperationId();
  ctx.mode = "software_zero";
  ctx.driver_yaml_path = path.toStdString();
  ctx.backup_path = backup.toStdString();
  ctx.operator_name = operator_edit_->text().toStdString();
  ctx.offsets = pending_offsets_;
  for (std::size_t i = 0; i < 4; ++i) ctx.snapshots.push_back(master_.snapshot(i));
  ctx.success = true;
  std::string error;
  const auto receipt_path = writeReceipt(config_.receipt_dir, ctx, &error);
  if (receipt_path.empty()) {
    appendLog("[回执][失败] " + QString::fromStdString(error));
  } else {
    appendLog("[回执] " + QString::fromStdString(receipt_path));
  }
  offsets_pending_ = false;
  setWorkflowEnabled(master_running_);
  QMessageBox::information(this, "标零完成",
      "motor_offset 已写入 YAML 并保存回执。\n"
      "请重启 taihu_steer_driver 验证 4 关节 q≈0（±0.5°）。");
}

}  // namespace taihu_zero
