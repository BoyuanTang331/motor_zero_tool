# Taihu 底盘转向电机标零工具（IgH 工控机版）

在底盘工控机上运行的 QT 标零工具：通过 IgH 内核主站（`ecat_com` / ethercat-cpp）
对 4 个串联的 Taihu 转向电机做零位标定。标零流程严格仿
`motion_control/src/merman_driver` 的 Ti5 HM35 装配调零状态机。

独立于 `slam_nav`：不需要启动 `taihu_steer_driver` 或任何 ROS 程序
（事实上标零时**必须停掉** taihu_steer_driver —— IgH master 同一时刻只能
被一个进程占用，启动脚本已内置此步骤）。

## 快速上手（新工控机 clone 三步走）

```bash
# 1. clone + 加启动脚本执行权限（仓库里脚本 mode=644，clone 后没有 x 位）
git clone https://github.com/BoyuanTang331/motor_zero_tool.git
cd motor_zero_tool
chmod +x launch/start_taihu_zero.sh

# 2. 装依赖 + 编译（NECRO 工控机；普通机器见下方"可移植性"）
sudo apt install -y build-essential cmake qt6-base-dev
cmake -B build -DDATP_DIR=/usr/necro    # 厂商库头文件在 /usr/necro/include
cmake --build build -j8

# 3. 一键标零（root 下执行；脚本自动：停驱动 → 起 IgH 主站 → 起工具 → 退出重启驱动）
sudo ./launch/start_taihu_zero.sh
```

> 仓库缺失 `config/NIIC_ENI_M0_steer.xml` 时 cmake 会报 file COPY 错误：
> 从底盘驱动产物拷贝补齐即可：
> `cp <slam_nav>/install/taihu_steer_driver/share/taihu_steer_driver/config/NIIC_ENI_M0_steer.xml config/`

## 一键启动脚本说明

`launch/start_taihu_zero.sh` 做四件事：

1. `pkill taihu_steer_driver`（IgH master 独占，标零必须独占总线）
2. `sudo ethercatctl start`（拉起 IgH 内核主站；**跳过此步直接点"启动主站"会
   因 /dev/EtherCAT0 不存在而段错误闪退**——这是工具最容易踩的坑）
3. 前台启动 QT 标零工具（带 `--config config/zero_tool.yaml`）
4. 工具退出后自动重启底盘驱动（`DRIVER_RESTART=false` 可跳过，见下）

常用变体：

```bash
sudo ./launch/start_taihu_zero.sh                              # 标准流程（驱动联动）
sudo DRIVER_RESTART=false ./launch/start_taihu_zero.sh         # 独立标零，不动驱动
```

运行要求：**必须 root**（qiuniu 实时运行时要 root 建实时线程，
普通用户报 Operation not permitted）；GUI 程序，在工控机桌面终端里跑
（纯 SSH 无 DISPLAY 起不来界面）。

## 界面标零流程（对应按钮 1~5）

1. 点 **1. 启动主站**（校验 4 轴拓扑，fail-closed）
2. **将 4 个转向轮摆到机械零位**（工装/对线）——物理操作，零位定义的基准
3. 点 **一键读编码盘值** 记录当前 counts（留档/对照，非必须）
4. 点 **2. HM35 探测**（SDO 读 0x6098，可读 = 固件支持硬件刷零）
5. **路线 A（推荐）**：点 **3. HM35 硬件刷零** → 零点写入编码器 + Flash 保存
   （0x1010 'save' / 0x2000 备选），**掉电不丢**；成功后驱动侧 motor_offset
   应归 `[0,0,0,0]`，**不需要第 4/5 步**
6. **路线 B（降级，探测不支持时）**：点 **4. 软件零位**（0x6064 采 50 帧取中值）
   → 点 **5. 写入 YAML+回执**（写回 `driver_yaml_path` 指向的驱动 yaml，自动备份）
7. 停止主站 → 关闭工具 → 脚本自动重启驱动 → 确认 4 关节 q≈0（±0.5°）

两条路线互斥二选一；JSON 回执只是审计凭证（谁/何时/从什么 counts 标零），
不是刷零本体。

## 可移植性：换一台新工控机能不能跑？

| 依赖 | 来源 | 缺失后果 |
|---|---|---|
| IgH 内核主站（ethercatctl / libethercat） | NECRO 工控机出厂镜像 | 普通 PC 装不了，工具无法运行（libethercat-cpp.so 为厂商定制版） |
| libqiuniu.so.2（厂商实时运行时） | 出厂镜像 /usr/necro/lib | ldd 直接解析失败，起不来 |
| datp/qiuniu 头文件 | 出厂镜像 /usr/necro/include | 可省略 `-DDATP_DIR`，仅编译警告（qiuniu_init 空实现），功能可用 |
| 4 个 Taihu 转向从站 + 网线 | 真机 | ENI 对表 4 从站，拓扑不符主站启动即报错 |
| ENI xml `config/NIIC_ENI_M0_steer.xml` | 随仓库 / 驱动 install 拷贝 | cmake 配置期报错（见快速上手） |
| slam_nav 驱动代码 | 底盘工控机才有 | **不影响 HM35 硬件刷零**；软件零位写 yaml 会失败；驱动联动收尾用 `DRIVER_RESTART=false` 跳过 |

结论：

- **同款 NECRO 工控机 + 电机接上**：clone → 编译 → `sudo ./launch/start_taihu_zero.sh`
  即可完成 HM35 标零，不依赖 slam_nav。
- **软件零位 / 自动重启驱动**：需要该机装着 slam_nav（yaml 路径在
  `config/zero_tool.yaml` 的 `driver_yaml_path`，按目标机实际路径改）。
- **普通 x86 电脑**：不可运行（无 IgH 内核主站与厂商库），只能改代码/编译交叉验证。

## 架构

```
QT 界面层 (mainwindow.cpp)      — 只读快照/触发任务，不做 EtherCAT 调用
  ↓ Qt::QueuedConnection
ZeroWorker (worker QThread)     — 全部 SDO/状态机（阻塞调用只在这里）
  ↓
cia402_zeroing (纯逻辑)          — 仿 ti5_homing_commissioning 的 7 步 HM 状态机
  ↓ 注入 ZeroSdoOps / ZeroPdoExchange
ZeroMaster (zero_master.cpp)    — ecat_com 封装：PDO 快照 + staged 输出 + SDO
  ↓
ecat_com (third_party/, 拷自 taihu_steer_driver)  — IgH ethercat-cpp 预编译库
  ↓
IgH 内核主站 (ethercatctl start)
```

## HM35 与软件零位的技术对应（与 Ti5/SOEM 一一对应）

| 步骤 | Ti5 (merman, SOEM) | 本工具 (Taihu, IgH) |
|---|---|---|
| 预检 | 控制字 0x0000 观察静止/无故障 | 相同（StationaryDisabled 判定） |
| 探测 | — | SDO 读 0x6098:00（固件不支持则走软件零位） |
| 方法写入 | SDO 写 0x6098=35 + 回读 | 相同 |
| 时序 | 0x0006/0 → 0x0086/0 → 0x0086/6 → 0x0006/6 → 0x0007/6 → 0x000F/6 → 0x001F/6 | 原样照搬（7 步 sequence_template） |
| 判定 | 无 Fault + 无 0x2000 + \|pos\|≤1 + vel=0 + mode=6，连续 3 帧，2s 超时 | 相同 |
| 收尾 | 0x0006/6 退 HM → 0x0006/8 恢复失能 CSP | 相同 |
| 失败遏制 | 0x0006/8 冻结目标位置 | 相同（failAndContain） |
| **Flash 保存** | SDO 写 0x2000:00=1 → 回读清零 | **0x1010:01 'save'（CiA 标准）→ 0x2000:00=1（备选），回读验证** |
| 回执 | 原子 JSON 记录 | 相同（仅审计凭证，不是刷零本体） |

**软件零位公式**（与 taihu_steer_driver main.cpp 完全一致）：
`q = (counts - offset) * dir / encoder_resolution * 2π`

## 编译细节（排障用）

- 厂商 datp/qiuniu 库：NECRO 工控机在 `/usr/necro`（头 include/qiuniu、库 lib/），
  编译加 `-DDATP_DIR=/usr/necro`；校验 `ldd third_party/ecat_com/lib/libethercat-cpp.so`
  应无 "not found"
- SDO 读取走 `task_->get_master_ptr()->sdo_upload<T>()`（task 类的同名非模板接口
  仅 16 位，且会遮蔽 master 模板版）；vendor/product 用 `task_->axis_vid()/axis_pid()`
  （slave_config_info 头文件只有前置声明，不可解引用）
- ZeroMaster 支持同一进程内 stop 后再次 start（start 会清空上一轮 PDO 句柄，
  stop 会 release master——缺一不可，否则悬垂指针段错误 / reserve EBUSY 闪退）

## 红线

- 运行工具前必须停 taihu_steer_driver（脚本已强制；手工运行同理）
- 必须先 `ethercatctl start` 再点"启动主站"（脚本已内置；否则闪退）
- 必须以 root 运行（qiuniu 实时运行时要求）
- SDO 只在 ZeroWorker QThread 调用（禁止 UI 线程；周期回调禁 SDO/禁碰 QT）
- 带电不插拔网线
- 标零后记得把软件零位写出的驱动 yaml 回拷到 slam_nav 源码并提交 git，
  防止下次 colcon build 用旧配置覆盖 install
