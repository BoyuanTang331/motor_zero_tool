#pragma once

// MainWindow — Taihu 转向电机标零工具界面
//
// 线程模型（红线）：
//   - ecat 周期线程（task 内建）：ZeroMaster::on_receive，不碰 QT；
//   - ZeroWorker（QThread）：所有 SDO 与状态机（阻塞调用），界面按钮触发；
//   - QT 主线程：100ms 定时器读 ZeroMaster 原子快照刷新卡片，不做任何 EtherCAT 调用。

#include <QMainWindow>
#include <QTimer>
#include <QThread>

#include <array>
#include <memory>
#include <string>

#include "cia402_zeroing.hpp"
#include "receipt.hpp"
#include "yaml_io.hpp"
#include "zero_master.hpp"

class QLabel;
class QPushButton;
class QTextEdit;
class QGroupBox;
class QLineEdit;

namespace taihu_zero {

struct ToolConfig {
  std::string eni_file;
  std::string driver_yaml_path;
  std::string receipt_dir;
  int master_id = 0;
  int cpu_affinity = 1;
  int priority = 90;
  std::int64_t cycle_time_ns = 5000000;
  double encoder_resolution = 262144.0;
  std::array<int, 4> motor_dir{-1, -1, -1, -1};
};

// 简单 key: value 解析（避免引入 yaml-cpp）
ToolConfig loadToolConfig(const std::string& path, std::string* error);

// ---- 后台任务（QThread 内运行） ----
class ZeroWorker : public QObject {
  Q_OBJECT
public:
  explicit ZeroWorker(ZeroMaster* master, QObject* parent = nullptr);
  void configure(std::size_t axis_count, const ZeroingOptions& options,
                 double encoder_resolution,
                 const std::array<int, 4>& motor_dir);

  Q_INVOKABLE void probeHm35();
  Q_INVOKABLE void runSoftwareZero();
  Q_INVOKABLE void runHm35Zero();
  Q_INVOKABLE void readEncoders();
  Q_INVOKABLE void writeOffsetsToYaml(const QString& yaml_path);
  Q_INVOKABLE void requestCancel();

signals:
  void logLine(const QString& text);
  void progress(int percent);
  void softwareOffsetsReady(const QStringList& offsets,
                            const QStringList& previews);
  void hm35ProbeDone(bool supported, const QString& detail);
  void hm35Done(bool ok, const QString& summary);
  void softwareDone(bool ok, const QString& summary);
  void encoderReadDone(const QStringList& lines);
  void yamlWritten(bool ok, const QString& path, const QString& backup);

private:
  ZeroObservation exchangeOnce(const ZeroCommand& command);
  ZeroingOptions options_;
  std::atomic<bool> cancel_{false};
  std::size_t axis_count_ = 4;
  ZeroMaster* master_;  // 非拥有
  double encoder_resolution_ = 262144.0;
  std::array<int, 4> motor_dir_{-1, -1, -1, -1};
  std::array<std::int32_t, 4> pending_offsets_{0, 0, 0, 0};
  ZeroingResult last_hm35_result_;
};

// ---- 主窗口 ----
class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(const ToolConfig& config, QWidget* parent = nullptr);
  ~MainWindow() override;

private slots:
  void onStartMaster();
  void onStopMaster();
  void onProbeHm35();
  void onReadEncoders();
  void onSoftwareZero();
  void onHm35Zero();
  void onWriteYaml();
  void refreshSnapshots();
  void appendLog(const QString& text);
  void onSoftwareOffsets(const QStringList& offsets,
                         const QStringList& previews);
  void onHm35Done(bool ok, const QString& summary);
  void onHm35Probe(bool supported, const QString& detail);
  void onSoftwareDone(bool ok, const QString& summary);
  void onEncoderRead(const QStringList& lines);
  void onYamlWritten(bool ok, const QString& path, const QString& backup);

private:
  void buildUi();
  void setWorkflowEnabled(bool master_running);
  QString axisCardText(std::size_t axis) const;

  ToolConfig config_;
  ZeroMaster master_;
  bool master_running_ = false;
  bool hm35_supported_ = false;
  std::array<std::int32_t, 4> pending_offsets_{0, 0, 0, 0};
  bool offsets_pending_ = false;

  QThread worker_thread_;
  ZeroWorker* worker_ = nullptr;
  QTimer refresh_timer_;

  // UI
  QLabel* status_label_ = nullptr;
  QPushButton* start_btn_ = nullptr;
  QPushButton* stop_btn_ = nullptr;
  QPushButton* read_btn_ = nullptr;
  QPushButton* probe_btn_ = nullptr;
  QPushButton* software_btn_ = nullptr;
  QPushButton* hm35_btn_ = nullptr;
  QPushButton* write_btn_ = nullptr;
  std::array<QLabel*, 4> axis_labels_{};
  QTextEdit* log_ = nullptr;
  QLineEdit* operator_edit_ = nullptr;
};

}  // namespace taihu_zero
