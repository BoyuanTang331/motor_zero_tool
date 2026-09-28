# Taihu 底盘转向电机 QT 标零工具 · 开发规划

> 版本 v0.2 · 2026-09-28 · 供 Kimi Coding 实现参考
> 参考对象: `D:\code\motion_control\src\merman_driver`（Ti5 HM35 调零流程, SOEM 主站, 只读参考, 不改动）
> 本工程独立于 motion_control, 所有产出物只写在本目录

## 1. 目标与约束

| 项 | 内容 |
|---|---|
| 目标 | Windows 桌面 exe（QT），对底盘 4 个 Taihu 转向电机做零位标定 |
| 交付物 | 绿色文件夹包（exe + DLL）+ 标定回执 JSON |
| 兼容性 | 产出的 `motor_offset` 直接写入 `taihu_steer_driver_config.yaml`，现有 IgH 驱动零改动 |
| 运行环境 | Win10/11 x64 + Npcap（SOEM win32 后端依赖 winpcap 兼容层） |
| 红线 | **只读参考 motion_control 与 fast_lio 空间, 不改动两库任何文件** |

## 2. 事实依据（代码证据, 全部只读）

### 2.1 底盘转向电机 = EtherCAT 网线串联（已确认）

- 代码位置: `D:\code\slam_nav\merman_slam\fast_lio\taihu_steer_driver\`
- ENI `config/NIIC_ENI_M0_steer.xml`: 4 个同型号从站线性串联
  - VendorId=5382695, ProductCode=37459, RevisionNo=65541
  - DC CycleTime0 = 5,000,000 ns（5ms）与 YAML `cycle_time_ns: 5000000` 一致
- 拓扑: 调试笔记本/工控机网卡 → 电机1 → 电机2 → 电机3 → 电机4（菊花链）

### 2.2 两套系统架构差异（关键）

| 维度 | 机械臂 Ti5 (motion_control) | 底盘 Taihu (fast_lio 空间) |
|---|---|---|
| 主站 | SOEM 用户态 | IgH 内核态 + ethercat-cpp 封装 |
| Windows 可用 | 是（soem/oshw/win32/wpcap 已有移植） | **否**（IgH 仅 Linux） |
| 配置方式 | 代码内拓扑校验 | ENI XML (NIIC_ENI_M0_steer.xml) |
| PDO | CiA402 CSP 标准映射 | **同款** CiA402 CSP 映射 |
| 零位方案 | HM35 硬件刷零 + Flash | 纯软件 `motor_offset` (YAML) |
| 编码器 | - | 18bit 单圈, resolution=262144 counts/rev |

结论: **QT 工具必须用 SOEM**（IgH 上不了 Windows），SOEM win32 源码从 motion_control 拷贝到本工程 `third_party/soem/`（拷贝即独立, 不反向依赖）。

### 2.3 Taihu PDO 映射（ecat_master.cpp:55-83）

- TxPDO (主站→从站): 0x6040 控制字, 0x607A 目标位置, 0x60FF 目标速度, 0x6071 目标扭矩, 0x6060 运行模式
- RxPDO (从站→主站): 0x6041 状态字, 0x6064 实际位置, 0x606C 实际速度, 0x6077 实际扭矩, 0x6061 模式显示, 0x603F 错误码

### 2.4 现有零位公式（main.cpp:467-469，工具必须兼容）

```
q_rad = (counts - motor_offset) * motor_dir / 262144 * 2π
```

- `motor_offset`: int[4]，YAML 当前 [0,0,0,0]，历史标定 [73967, 22595, -63344, 42586]
- `motor_dir`: [-1,-1,-1,-1]
- 标定算法: 电机摆到机械零位 → 读 0x6064 原始 counts → `offset = counts`（此时 q=0）

## 3. 工具架构（三层）

```
┌─────────────────────────────────────────────┐
│ QT 界面层 (qwidgets)                         │
│  网卡选择 / 从站扫描表格 / 4电机实时状态卡片    │
│  标零向导(下一步式) / 日志面板 / 回执查看      │
├─────────────────────────────────────────────┤
│ 标零逻辑层 (纯 C++, 无 QT 依赖, 可单测)       │
│  v1: 软件零位 — 预检→读counts→算offset→写YAML │
│  v2: HM35 探测 — 移植 ti5_homing_commissioning│
│      7步状态机(0x0006/0→...→0x001F/6)        │
├─────────────────────────────────────────────┤
│ SOEM 通讯层 (拷贝 motion_control/third_party/soem)│
│  ecx_config_init 扫描 / ecx_SDOwrite/read     │
│  ecx_send/receive_processdata 周期帧          │
│  win32: npcap/winpcap 后端                    │
└─────────────────────────────────────────────┘
```

## 4. 分阶段验证计划（mock 先行）

| 阶段 | 做什么 | 如何验证 | 通过标准 | 流产信号 |
|---|---|---|---|---|
| M1 | SOEM win32 编译 + 网卡枚举 + 从站扫描 | Windows 上连真实底盘, 只读 | 4 从站全部识别, vendor/product 与 ENI 一致 | 从站数量≠4 / vendor 不符 |
| M2 | SDO 读写（不进 OP、不使能） | 读 0x6064/0x6041/0x6061 | 位置读数稳定(静止时抖动≤2 counts), 状态字有效 | SDO 超时 / 读数全零 |
| M3 | v1 软件零位流程 | 电机手动摆零位 → 工具标定 → 写 YAML → 启 taihu_steer_driver 验证 | 驱动启动后 4 关节 q≈0 (±0.5°) | q 偏差大 / 驱动报错 |
| M4 | 界面完善 + 回执 JSON + 绿色包打包 | 现场操作员走一遍向导 | 解压即用, 回执含 before/after/时间戳 | 缺 DLL / 回执丢失 |
| M5 (可选) | HM35 硬件刷零探测 | SDO 试读 0x6098, 写 35, 回读 | 回读=35 则固件支持, 可走完整 HM 流程 | 读/写失败 → 固件不支持, 保持 v1 方案 |

## 5. v1 标零流程（软件零位, 与现有驱动哲学一致）

1. 选网卡 → `ecx_config_init` 扫描 → 校验 4 从站 (数量/vendor/product)
2. **预检**: 4 电机静止（0x606C 速度=0, 0x603F 无错误, 状态无 Fault）——全程不使能电机
3. 操作员将 4 转向轮摆到机械零位（工装/对线）, 点"采集"
4. PRE-OP 下 SDO 连续读 0x6064 ≥50 次, 取中值 counts[i]（抗抖动）
5. `motor_offset[i] = counts[i]`, 预览 `q=0` 效果
6. 写回 `taihu_steer_driver_config.yaml` 的 `motor_offset` 字段（保留原值注释）
7. 另存 JSON 回执: 时间戳 / 网卡 / 4 从站 serial / before-after counts / 操作员
8. 关闭网卡句柄退出

## 6. 风险清单

| 风险 | 等级 | 对策 |
|---|---|---|
| Taihu 固件 0x6098 不支持 HM35 | 高(v2) | v1 不依赖; M5 先探测再决定 |
| Windows 网卡时序抖动 | 低(v1) | v1 走 SDO 非周期访问, 与实时性无关 |
| 直连时底盘 IgH 主站同时在跑 | 高 | 网线物理隔离 + 工具检测从站状态, 见 8.6 |
| offset 写错 YAML 格式 | 低 | 只改 motor_offset 一行, 写前备份 .bak |
| 带电插拔损坏网口 | 低 | 文档注明先断电后插拔 |

## 7. 代码复用清单（从 motion_control 拷贝进本工程, 源库不动）

| 源 | 用途 |
|---|---|
| `src/third_party/soem/` 整目录 | SOEM 库, 含 oshw/win32 |
| `src/merman_driver/src/ethercat/soem_ti5_sdo_*.cpp` | SDO 函数指针封装模式 |
| `src/merman_driver/src/ethercat/ti5_homing_commissioning.cpp` | v2 HM35 状态机 + 稳定帧判定 + 失败遏制模式 |
| `src/merman_driver/src/evidence/atomic_record_writer.cpp` | 原子 JSON 回执写入模式 |

## 8. 部署形态决策（2026-09-28 定稿）

### 8.1 使用场景约束

- 测试员工用 Win 笔记本 + 网线直连 4 个串联电机（菊花链）
- 测试机**零环境假设**: 无 C++ 运行库、无 QT、无开发工具, 只有裸 Windows
- 操作员只想双击一个东西

### 8.2 三种形态对比

| 维度 | A. 绿色文件夹包 (推荐) | B. QT 静态编译单 exe | C. 工控机 IgH 工具 |
|---|---|---|---|
| 形态 | exe + Qt DLL + 插件, zip 解压即用 | 真单文件 | Linux 程序, 装在工控机 |
| C++ 运行库 | MSVC /MT 静态链接进去, 免装 | 同左, 免装 | 无此问题 |
| QT 依赖 | windeployqt 自动收齐 | 编译期静态链接, 无 DLL | 无此问题 |
| **Npcap（唯一绕不开）** | 首次运行检测 → 引导安装 | 同左, **装不进 exe** | 无需 (IgH 内核态) |
| 打包工作量 | 低 (windeployqt + zip, 半天) | 高 (自编译 QT 静态库, 小一天) | 中 (复用 ecat_com, 1-2 天) |
| 测试员工体验 | 解压→双击→一次性装 npcap→完 | 同左, 少一步解压 | 要在工控机上操作/停驱动 |
| 主站冲突 | 物理隔离: 网线从工控机拔下插笔记本 | 同左 | 需先停 taihu_steer_driver |
| Windows 时序风险 | 有 (非实时, 但标零低带宽可容忍) | 同左 | 无 |

**结论: 主方案 A, 备胎 C。**

### 8.3 Npcap 为什么装不进 exe（硬约束）

SOEM 在 Windows 上发/收原始以太网帧的**唯一通道**是 winpcap 兼容 API（packet.dll/wpcap.dll → npcap 驱动）。这是**内核级网络驱动**，任何"绿色/静态"手段都绕不开系统安装，必须管理员权限装一次（约 2 分钟，装完永久生效）。Windows 没有免驱动的原始帧 API。

对策（工具内置引导逻辑）:
1. 启动时 `LoadLibrary("wpcap.dll")` 探测
2. 失败 → 弹窗"缺少 Npcap 驱动" + 内嵌的 `npcap-x.x.exe` 安装器一键调起（UAC 提权）
3. 安装完成自动重试, 网卡枚举出现即放行
4. 绿色包目录: `taihu_zero_tool/`（exe + DLLs/ + platforms/ + npcap-installer/）

### 8.4 降低 Windows 时序风险的技术选择（v1 专项）

v1 软件零位标定**不进 OP、不使能电机**:

- PRE-OP 状态下用 **SDO 读 0x6064**（对象字典访问, 非周期, 无实时性要求）
- 标定采集 = SDO 连续读 50 次取中值, Windows 调度抖动完全无影响
- 界面上仍可 1-2 Hz 低频轮询 SDO 显示 4 电机实时位置/状态字
- 进 OP + PDO 周期帧仅在 v2 (HM35) 才需要, 且 Ti5 状态机本身带 2s 超时 + 3 帧稳定判定, 对抖动天然免疫

### 8.5 工控机 IgH 备胎方案 (Plan C) 边界

- 仅当笔记本网卡与 npcap 兼容性出问题时启用
- 实现: 复用 `taihu_steer_driver/ecat_com` + 现成 ENI, 写独立 CLI/简单 GUI（独立目录, 不改 fast_lio 库）
- 前置: `stop_pattern_gracefully "taihu_steer_driver"` → 跑工具 → 重启驱动
- 回执 YAML 写回逻辑与 Win 版共享同一份核心代码 (标零逻辑层设计成平台无关)

### 8.6 双主站防冲突（安全红线）

笔记本直连标零时**必须**先把底盘网线从工控机拔下（或确认 IgH 已停）。
工具在 `ecx_config_init` 后校验: 从站数=4 且全 PRE-OP/INIT; 若从站已进 OP → 判定为"别的主站在跑", 拒绝操作并提示。

## 9. 机械臂对"抖动"的真实解法（代码考古结论, 2026-09-28）

### 9.1 部署位置: 就在机器人本体的上肢控制器上刷零

- `scripts/startup_upper_motion.sh`: 工作区 `/home/ubt2204/motion_control`, `sudo ros2 launch merman_driver`, 调零 GUI 也装在本机（`install_merman_commissioning_desktop.sh`）
- 证据文件 `docs/communication_runtime_f3_evidence_20260918.yaml`: 控制器 = **Intel Core i7-1360P, Ubuntu 22.04.5, kernel 6.8.0-138-generic**
- 关键: **generic 内核, 不是 PREEMPT_RT**。没有 RT 补丁, 没有_rt 后缀

### 9.2 抖动处理 = "隔离 + 容忍", 不是"消除"

| 层 | 机制 | 代码位置 |
|---|---|---|
| 线程隔离 | EtherCAT motor 线程**独占 CPU6**（planning 2-5, estimation 11, ROS executor 12-14 各自隔离） | config/runtime_config.yaml:35-39 |
| 优先级 | motor 线程 **SCHED_FIFO/95**（默认, E2_MOTOR_PRIO 可覆盖）; 绑定失败=致命退出, FIFO 申请失败=降级继续+告警 | src/runtime/thread_affinity.cpp:495, ethercat_session.cpp:2791-2852 |
| 周期 | 5ms 循环, `clock_nanosleep(CLOCK_MONOTONIC)` 相对延时 | EthercatMaster.cpp:363-368 |
| 协议容忍 | 每帧校验 WKC; HM 状态机每阶段**连续 3 帧稳定**才算通过、2s 超时; Homed 判定容忍 ±1 count; 帧失败→失败遏制而非崩溃 | ti5_homing_commissioning.cpp (pollStage) |
| 监控 | cyclic diagnostics 快照记录循环健康 | ethercat_cyclic_diagnostics.cpp |

### 9.3 对本工具的启示

- 机械臂在 generic 内核上靠 SCHED_FIFO + 核隔离就把 5ms 循环跑稳了 → **抖动靠设计容忍, 不靠硬实时消除**
- Windows 上没有 SCHED_FIFO, 但 v1 走 SDO（非周期）完全无影响; v2 HM35 移植时**保留 3 帧稳定 + 2s 超时 + WKC 校验**的容忍逻辑即可, 必要时可用 MMCSS (avSetMmThreadCharacteristics) 提升线程优先级, 但不是必需
