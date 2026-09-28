#include <QApplication>

#include <iostream>
#include <string>

#include "mainwindow.hpp"

// 厂商实时运行时初始化（照抄 taihu_steer_driver main.cpp: qiuniu_init()）
// 头文件来自工控机厂商 datp 安装包；本机没有时跳过（libethercat-cpp.so
// 内部仍会在运行时自行解析，缺库则链接期/启动期报错，见 README）。
#if __has_include(<qiuniu/init.h>)
#include <qiuniu/init.h>
#define TAIHU_ZERO_HAS_QIUNIU 1
#else
inline void qiuniu_init() {}
#define TAIHU_ZERO_HAS_QIUNIU 0
#endif

// 用法:
//   taihu_zero_tool [--config <zero_tool.yaml>]
// 默认读取可执行文件同级 config/zero_tool.yaml（构建时自动拷贝）。

int main(int argc, char** argv) {
  // 与 taihu_steer_driver 相同：启动任何 EtherCAT 对象之前先初始化厂商运行时
  qiuniu_init();
  if (!TAIHU_ZERO_HAS_QIUNIU) {
    std::cerr << "[taihu_zero_tool] 警告: 未找到 <qiuniu/init.h>，"
                 "qiuniu_init() 为空实现；若启动主站失败请检查 datp 库路径\n";
  }

  QApplication app(argc, argv);

  std::string config_path;
  for (int i = 1; i + 1 < argc; ++i) {
    if (std::string(argv[i]) == "--config") config_path = argv[i + 1];
  }
  if (config_path.empty()) {
    config_path =
        QApplication::applicationDirPath().toStdString() + "/config/zero_tool.yaml";
  }

  std::string error;
  auto config = taihu_zero::loadToolConfig(config_path, &error);
  if (!error.empty()) {
    std::cerr << "[taihu_zero_tool] 配置错误: " << error << std::endl;
  }

  taihu_zero::MainWindow window(config);
  window.show();
  return QApplication::exec();
}
