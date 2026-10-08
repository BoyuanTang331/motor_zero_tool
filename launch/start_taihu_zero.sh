#!/bin/bash
# Taihu 转向电机标零工具 · 现场启动脚本
# 流程：停止底盘驱动（IgH master 独占）→ 启动 IgH 主站 → 启动 QT 标零工具
# 退出工具后自动重启底盘驱动。

set -u

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

TOOL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOL_BIN="${TOOL_DIR}/build/taihu_zero_tool"
CONFIG_FILE="${TOOL_DIR}/config/zero_tool.yaml"
DRIVER_RESTART="${DRIVER_RESTART:-true}"
DRIVER_LAUNCH="${DRIVER_LAUNCH:-ros2 launch taihu_steer_driver taihu_steer_driver.launch.py}"
DRIVER_LOG="/tmp/taihu_zero_driver_restart.log"

if [ ! -x "$TOOL_BIN" ]; then
  echo -e "${RED}[错误] 工具未编译: $TOOL_BIN${NC}"
  echo "先执行: cd $TOOL_DIR && cmake -B build && cmake --build build -j"
  exit 1
fi

echo -e "${GREEN}=== Taihu 转向电机标零工具启动 ===${NC}"

# 1) 停止底盘驱动（IgH master 同一时刻只能被一个进程 reserve）
echo -e "${YELLOW}[1/3] 停止 taihu_steer_driver（IgH master 独占）...${NC}"
pkill -INT -f "taihu_steer_driver" 2>/dev/null || true
sleep 3
pkill -TERM -f "taihu_steer_driver" 2>/dev/null || true
sleep 1
if pgrep -f "taihu_steer_driver" >/dev/null 2>&1; then
  echo -e "${RED}[错误] taihu_steer_driver 仍在运行，拒绝启动标零工具${NC}"
  exit 1
fi
echo -e "${GREEN}  ✓ 底盘驱动已停止${NC}"

# 2) 启动 IgH 主站
echo -e "${YELLOW}[2/3] 启动 IgH EtherCAT 主站...${NC}"
if command -v ethercatctl >/dev/null 2>&1; then
  sudo ethercatctl start 2>/dev/null || true
  sleep 1
  sudo ethercatctl status 2>/dev/null | grep -qi "running" \
    && echo -e "${GREEN}  ✓ IgH 主站运行中${NC}" \
    || echo -e "${YELLOW}  ⚠ ethercatctl 未确认 running，继续尝试${NC}"
fi

# 3) 启动 QT 标零工具（前台运行；退出后按开关重启驱动）
echo -e "${YELLOW}[3/3] 启动标零工具...${NC}"
"$TOOL_BIN" --config "$CONFIG_FILE"
TOOL_EXIT=$?

if [ "$DRIVER_RESTART" = "true" ]; then
  echo -e "${YELLOW}[收尾] 重启底盘驱动...${NC}"
  # 恢复 ROS2 环境（与 bringup_chassis.sh 一致）
  # set -u 与 ROS setup.bash 冲突（AMENT_TRACE_SETUP_FILES: unbound variable），
  # source 前后临时关闭 nounset；WS 默认指向 slam_nav 实际工作空间
  set +u
  [ -f "/opt/ros/humble/setup.bash" ] && source "/opt/ros/humble/setup.bash"
  WS="${ROS2_WS:-/home/niic/slam_nav/merman_slam/fast_lio}"
  [ -f "$WS/install/setup.bash" ] && source "$WS/install/setup.bash"
  set -u
  nohup bash -c "$DRIVER_LAUNCH" >"$DRIVER_LOG" 2>&1 &
  echo -e "${GREEN}  ✓ 底盘驱动已在后台重启（日志: $DRIVER_LOG）${NC}"
  echo -e "${YELLOW}  提示: 请确认 4 关节 q≈0（±0.5°）以验证标零效果${NC}"
fi

exit $TOOL_EXIT
