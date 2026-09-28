# Taihu 转向电机 QT 标零工具 · IgH 工控机方案（主方案 v2）

> 2026-09-28 思路变更: 弃"Win 笔记本 SOEM"为主方案, 改为**底盘工控机 Linux + IgH 内核主站 + QT 界面**。
> 本文档取代 `taihu_steer_zero_tool_plan.md` 的主方案地位（旧文档保留作 Win/SOEM 备胎参考）。
> 开发模式: 本地 Win 笔记本写代码, 工控机编译运行。
> 仿照对象: `motion_control/src/merman_driver` 分层写法（只读参考）。

## 1. 为什么这个方案更顺

| 对比项 | Win+SOEM（旧主案） | IgH+QT（新主案） |
|---|---|---|
| 运行位置 | 测试笔记本 | 底盘工控机（与 taihu_steer_driver 同机） |
| 主站 | SOEM 用户态+npca 驱动 | IgH 内核态, `ethercatctl start` 现成 |
| 通讯封装 | 要自己移植 Ti5 的 SOEM 层 | **ecat_com 现成, EcatMaster 类直接复用** |
| Npcap 安装问题 | 存在 | 不存在 |
| 时序 | Windows 抖动风险 | task 自带 SCHED_FIFO/CPU 绑核, 与现有驱动同级 |
| 测试员工零依赖诉求 | 需解压+装 npcap | 工控机常驻, 做成桌面图标双击 |
| 代价 | — | 标零时必须**停 taihu_steer_driver**（IgH master 同一时刻只能被一个进程占用） |

## 2. 代码事实（ecat_com 能力盘点, 全部只读确认）

`ecat::task`（task.hpp）能力:
- `load_eni(path, cycle_time)` — ENI 模式, 自动按 XML 对表 vendor/product/PDO/SM/DC
- `priority(prio)` / `cpu_affinity(cpus)` — **内建实时线程**, 现有配置 priority=90 / cpu_affinity=1
- `set_config_callback` — 从站配置完成后回调（EcatMaster 在这里注册 4 轴 PDO）
- `set_receive_callback` / `set_send_callback` — 每个通讯周期回调（周期循环入口）
- `sdo_download / sdo_upload` — **阻塞式** SDO 读写（注释明确: 不可在实时上下文调用）
- `start() / break_() / wait()` — 自动 configure→activate→周期线程
- `slave_count()` / `get_slave_info` — 拓扑校验

`EcatMaster`（taihu_steer_driver/src/ecat_master.cpp）已实现:
- 4 轴 PDO 注册（0x6040/607A/60FF/6071/6060/6041/6064/606C/6077/6061/603F）
- CiA402 使能状态机（enable_slave, 分段控制字 0x06→0x07→0x0F）
- `set_motor_config(offset, dir, n)` — 偏移/方向注入
- 电机方向错误保护、急停处理

→ 结论: **标零工具的通讯层 = 直接实例化 EcatMaster（拷贝/链接 ecat_com + ecat_master.cpp）**, 不用重写。

## 3. 工具架构（仿 merman_driver 分层, Linux）

```
┌──────────────────────────────────────────────────┐
│ QT 界面层 (QT Widgets, 工控机桌面)                │
│  4 电机状态卡片 / 标零向导 / 日志 / 回执查看       │
│  只做 UI, 不碰 EtherCAT                          │
├──────────────────────────────────────────────────┤
│ 标零逻辑层 (纯 C++, 无 QT 依赖, 可单测)           │
│  预检 / SDO 采集中值 / offset 计算 / YAML 写回    │
│  v2: HM35 状态机 (平移 ti5_homing_commissioning  │
│      的 7 步控制字 + 3 帧稳定 + 2s 超时 + 失败遏制)│
├──────────────────────────────────────────────────┤
│ 通讯层 = 复用 taihu_steer_driver                  │
│  ecat_com (ecat::task: ENI/SDO/周期线程)          │
│  EcatMaster (PDO 注册 + 使能状态机 + offset 管理) │
├──────────────────────────────────────────────────┤
│ IgH 内核主站 (ethercatctl start, /dev/EtherCAT0) │
└──────────────────────────────────────────────────┘
```

### 3.1 线程模型（关键工程点, 仿 merman 的隔离哲学）

| 线程 | 谁启动 | 干什么 | 禁忌 |
|---|---|---|---|
| QT 主线程 | QApplication | UI 渲染、向导交互 | 不调任何阻塞 EtherCAT 函数 |
| QT worker (QThread) | 界面触发 | 调 sdo_upload/sdo_download（阻塞数十 ms 级） | 不在 UI 线程调 |
| ecat 周期线程 | task.start() | 5ms send/receive 回调 | 回调内不做 SDO/不加锁/不碰 QT |
| 数据通道 | — | PDO 原子变量快照 + 无锁队列 | QT 只读快照, 周期线程只写 |

与 merman_driver 的对应: motor 线程(SCHED_FIFO/95, CPU6) ↔ ecat 周期线程(priority 90, CPU1); ROS executor ↔ QT UI; 二者隔离方式一致。

### 3.2 仿写的核心结构（对照表）

| merman_driver (Ti5/SOEM) | 本工具 (Taihu/IgH) |
|---|---|
| `ecx_config_init` 扫描+拓扑校验 | `task.load_eni()` + `slave_count()==4` + vendor 校验 |
| `ecx_SDOread/write` 函数指针 | `task.sdo_upload<T>/sdo_download` 模板 |
| IOmap + `htoel` 手动序列化 | `try_register_pdo_entry` 自动映射到 axis 结构体 |
| `precise_usleep` 5ms 节拍 | task 内建周期线程 |
| 3 帧稳定 + 2s 超时 + WKC 校验 | **逻辑原样平移**（WKC 换成 domain state / working counter 判断） |
| 失败遏制: 0x0006/8 回失能 CSP | 同样用 PDO 写控制字, ecat_com 下自动 |
| 原子 JSON 回执 | 同款（时间戳/serial/before/after/操作员） |
| `motorOffsetCounts()==0` 硬前置 | `set_motor_config` 注入的 offset 当前值必须为已知 |

## 4. 标零流程（v1 软件零位）

1. **前置脚本**: `stop taihu_steer_driver`（同一 IgH master 不能双进程 reserve）→ `ethercatctl start`
2. QT 启动 → EcatMaster init(ENI) → task.start() → 校验 4 从站 OP
3. 预检: 4 轴静止（0x606C=0, 0x603F=0, 状态字无 Fault）, 不使能
4. 操作员摆机械零位 → 点"采集"
5. QT worker 线程: `sdo_upload<int32>(pos, {0x6064,0})` × 50 次取中值 × 4 轴
6. `motor_offset[i] = counts[i]`, 预览 q=0
7. 写回 `taihu_steer_driver_config.yaml`（备份 .bak, 只动 motor_offset 行）
8. JSON 回执 + 重启 taihu_steer_driver 验证 q≈0（±0.5°）

## 5. 开发工作流（Win 笔记本 → Linux 工控机）

| 环节 | 做法 |
|---|---|
| 写代码 | Win 笔记本 QT Creator / VS Code + CMake 工程（代码放 `D:\code\taihu_zero_tool\`, git 管理） |
| 同步 | git push/pull 或 rsync 到工控机 `~/taihu_zero_tool` |
| 编译 | **在工控机上** `cmake -B build && cmake --build build`（ecat_com/IgH 用户库只有 Linux 有; WSL2 能编译但没必要） |
| 单测 | 标零逻辑层（纯 C++）可在 Win/WSL 跑 gtest; 通讯层只能在工控机 |
| 运行 | 工控机本机屏 / VNC / `ssh -X` X11 转发到笔记本显示 |
| 打包 | 工控机 apt 装 qt6-base 后直接跑; 或 linuxdeployqt/AppImage 做免依赖（可选） |
| 现场入口 | `.desktop` 桌面图标（内嵌前置脚本: 停驱动→ethercatctl start→起 QT） |

依赖清单（工控机）: g++, cmake, qt6-base-dev（或 qt5）, libyaml-cpp, git。IgH 主站与 ecat_com 已在机上。

## 6. 分阶段验证（M1–M5, 对齐 merman 的节奏）

| 阶段 | 做什么 | 如何验证 | 通过标准 | 流产信号 |
|---|---|---|---|---|
| M1 | 工控机编译 ecat_com + 最小 main: load_eni→start→打印 4 从站 | 只读, 不接运动 | slave_count=4, vendor=5382695/product=37459 | 数量≠4 / 对表失败 |
| M2 | SDO 读 0x6064/0x6041 + 周期回调采 PDO 快照 | 手转轮子看位置变化 | 读数连续、静止抖动≤2 counts | SDO 超时 / 数据恒 0 |
| M3 | v1 软件零位 CLI 版（无 QT） | 摆零→采集→写 YAML→重启驱动 | 4 关节 q≈0 (±0.5°) | q 偏差 / YAML 写坏 |
| M4 | QT 界面 + 线程模型 + 回执 + 桌面入口 | 操作员走全流程 | 双击图标完成标零 | UI 卡死（SDO 在 UI 线程调了）/ 驱动未停导致 reserve 失败 |
| M5 | HM35 探测 + v2 状态机（可选） | SDO 试 0x6098 | 回读=35 则做完整 HM 流程 | 不支持 → 停在 v1 |

## 7. 风险与红线

| 风险 | 等级 | 对策 |
|---|---|---|
| **双进程抢 master**（驱动没停就起工具） | 高 | 工具入口脚本强制 pkill taihu_steer_driver + reserve 失败时明确报错 |
| SDO 在 QT UI 线程调用 → 界面冻结 | 高 | 全部 SDO 走 QThread worker（代码评审红线） |
| ENI 与实际拓扑不一致 | 中 | task 对表失败即拒绝继续（fail-closed, 学 merman） |
| v2 HM35 固件不支持 | 中 | v1 兜底, M5 先探测 |
| offset 写坏 YAML | 低 | 备份 + 只改一行 + 写后回读校验 |
| 标零后忘重启驱动 | 低 | 工具收尾提示 + 回执记录驱动重启验证结果 |
