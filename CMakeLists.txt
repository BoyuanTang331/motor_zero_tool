cmake_minimum_required(VERSION 3.16)
project(taihu_zero_tool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

# Qt 界面（Qt5/Qt6 皆可）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------
# ecat_com（IgH ethercat-cpp 预编译库，从 taihu_steer_driver 拷贝，只读使用）
# 不走其官方 config.cmake（会级联 find_dependency Necro/Boost/spdlog），
# 直接导入 .so + include，qiuniu 由工控机系统库提供（与 taihu_steer_driver 相同）。
# ---------------------------------------------------------------------------
set(ECAT_COM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ecat_com")

add_library(ecat_cpp SHARED IMPORTED GLOBAL)
set_target_properties(ecat_cpp PROPERTIES
  IMPORTED_LOCATION "${ECAT_COM_DIR}/lib/libethercat-cpp.so")

# ---------------------------------------------------------------------------
# qiuniu / datp（工控机厂商实时运行时库，libethercat-cpp.so 的硬依赖）
# 引用方式照抄 taihu_steer_driver/CMakeLists.txt：
#   include_directories(.../datp/datp) + link_directories(.../datp)
#   + file(GLOB DATP_LIBS ".../datp/libdatp.so")
# 本地仓库不带 datp 目录（厂商安装在工控机上），因此用 -DDATP_DIR= 或
# 系统标准路径探测；代码侧用 __has_include(<qiuniu/init.h>) 保护。
# ---------------------------------------------------------------------------
set(DATP_DIR "" CACHE PATH "厂商 datp 库安装目录（含 datp/qiuniu 头文件与 libdatp.so）")
if(DATP_DIR)
  include_directories("${DATP_DIR}/datp")
endif()

find_library(DATP_LIBRARY NAMES datp
  PATHS ${DATP_DIR} /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
        /home/niic/datp /opt/datp)
find_library(QIUNIU_LIBRARY NAMES qiuniu
  PATHS ${DATP_DIR} /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
if(NOT DATP_LIBRARY AND NOT QIUNIU_LIBRARY)
  message(WARNING "libdatp/libqiuniu 均未找到；配置仍可继续（代码侧有 __has_include 保护），"
                  "但运行时 ldd libethercat-cpp.so 必须能解析到厂商库，"
                  "否则需 -DDATP_DIR=<工控机上 datp 安装目录> 重新配置")
endif()

add_executable(taihu_zero_tool
  src/main.cpp
  src/zero_master.cpp
  src/cia402_zeroing.cpp
  src/yaml_io.cpp
  src/receipt.cpp
  src/mainwindow.cpp)

target_include_directories(taihu_zero_tool PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${ECAT_COM_DIR}/include")

target_link_libraries(taihu_zero_tool PRIVATE
  Qt${QT_VERSION_MAJOR}::Widgets
  ecat_cpp
  Threads::Threads)
# 厂商库：优先 datp（taihu 驱动的引用名），其次 qiuniu
if(DATP_LIBRARY)
  target_link_libraries(taihu_zero_tool PRIVATE ${DATP_LIBRARY})
elseif(QIUNIU_LIBRARY)
  target_link_libraries(taihu_zero_tool PRIVATE ${QIUNIU_LIBRARY})
endif()

# 工具配置文件（构建目录可见）
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config/zero_tool.yaml
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config/NIIC_ENI_M0_steer.xml
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config)
