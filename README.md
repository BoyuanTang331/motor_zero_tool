# Taihu 底盘转向电机标零工具（IgH 工控机版）

在底盘工控机上运行的 QT 标零工具：通过 IgH 内核主站（`ecat_com` / ethercat-cpp）
对 4 个串联的 Taihu 转向电机做零位标定。标零流程严格仿
`motion_control/src/merman_driver` 的 Ti5 HM35 装配调零状态机。

独立于 `slam_nav`：不需要启动 `taihu_steer_driver` 或任何 ROS 程序
（事实上标零时**必须停掉** taihu_steer_driver —— IgH master 同一时刻只能
被一个进程占用，启动脚本已内置此步骤）。

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

## 标零流程（与 Ti5/SOEM 一一对应）

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

**HM35 硬件刷零（默认推荐路径）**：SDO 写 0x6098=35 → 7 步控制字时序把
"当前位置设为编码器零点" → **Flash 保存（0x1010 / 0x2000）掉电不丢**。
成功后驱动侧 `motor_offset` 应归 `[0,0,0,0]`。
⚠️ 不做 Flash 保存的 HM35 零点可能是**易失的**（掉电丢失）——工具已内置
保存步骤并逐轴回报结果。

**软件零位（降级备选，固件不支持 HM 时用）**：SDO 读 0x6064 × 50 次取中值
→ `motor_offset[i] = counts[i]` → 写回 `taihu_steer_driver_config.yaml`
（自动备份）→ 重启驱动验证 q≈0。
公式与驱动完全一致：`q = (counts - offset) * dir / 262144 * 2π`。

**JSON 回执的角色**：只是"谁在什么时间把哪些轴从什么 counts 标成了零"
的审计凭证（便于追溯与防重放），刷零动作本体是上面的 HM35+Flash 或
YAML 写回，与 JSON 无关。

## 工控机首次部署

```bash
# 依赖（工控机只需一次）
sudo apt install -y build-essential cmake qt6-base-dev
# 若 Qt6 不可用（Ubuntu 22.04 默认有 qt6-base-dev）
# 备选 Qt5: sudo apt install -y qtbase5-dev

# 厂商 datp/qiuniu 库（工控机厂商实时运行时，libethercat-cpp.so 硬依赖）：
# 引用方式与 taihu_steer_driver 相同（include <qiuniu/init.h> + 链接 libdatp.so），
# 厂商安装在工控机上。cmake 时用 -DDATP_DIR=<datp 安装目录> 指定；
# 不指定时自动探测标准路径，找不到仅警告（代码侧有 __has_include 保护）。
# 校验: ldd third_party/ecat_com/lib/libethercat-cpp.so 应无 "not found"

# 编译
cd ~/taihu_zero_tool
cmake -B build -DDATP_DIR=/home/niic/datp   # 按工控机实际路径
cmake --build build -j

# 编辑配置（ENI 路径、驱动 YAML 路径、回执目录）
vim config/zero_tool.yaml

# 安装桌面入口
cp launch/taihu-zero-tool.desktop ~/.local/share/applications/
```

## 使用

双击桌面图标（或 `bash launch/start_taihu_zero.sh`）：
1. 脚本自动停止 taihu_steer_driver + 启动 IgH 主站
2. QT 界面点 **1. 启动主站**（校验 4 轴拓扑，fail-closed）
3. 将 4 个转向轮摆到机械零位（工装/对线）
4. 点 **一键读编码盘值** 记录当前 counts（留档/对照）
5. 点 **2. HM35 探测**（0x6098 可读则固件支持硬件刷零）
6. 点 **3. HM35 硬件刷零** → 零点写入编码器 + Flash 保存，掉电不丢
7. 退出工具 → 脚本自动重启驱动 → 确认 4 关节 q≈0（±0.5°）

探测不通过时降级：点 **4. 软件零位** → **5. 写入 YAML+回执**。

## 红线

- 运行工具前必须停 taihu_steer_driver（脚本已强制；手工运行同理）
- SDO 只在 ZeroWorker QThread 调用（禁止 UI 线程；周期回调禁 SDO/禁碰 QT）
- 带电不插拔网线
