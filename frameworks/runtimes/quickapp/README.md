# QuickApp 应用容器

\[ [English](README_en.md) | 简体中文 \]

## 概述

QuickApp 应用容器是 [openvela](https://github.com/open-vela) 系统上的[快应用](https://doc.quickapp.cn/)运行时框架，为 AIoT 设备（如智能手表、智能音箱、智能面板等）提供轻量级的快应用执行环境。

本项目基于 [QuickJS](https://bellard.org/quickjs/) JavaScript 引擎，结合 [LVGL](https://lvgl.io/) 图形库和 [Yoga](https://yogalayout.dev/) 排版引擎，实现了快应用标准中定义的页面渲染、路由管理、生命周期控制等核心能力。

主要功能包括：

- 快应用 RPK 包的加载与运行。
- 基于 LVGL 的 GUI 渲染与组件体系。
- 页面路由与导航栈管理。
- 应用生命周期管理（创建、前台、后台、销毁等）。
- JavaScript Feature 扩展机制。
- 基于 CDP 协议的远程调试（Inspector）支持。

> **说明：** 本仓库以预编译库（Prebuilt）形式发布核心模块，同时开放头文件与 Shell 集成层源码，供开发者进行应用集成与二次开发。

## 架构图

```
┌─────────────────────────────────────────────────┐
│                  Shell 集成层                     │
│         (vapp / vappxms 应用入口)                 │
├─────────────────────────────────────────────────┤
│                 QuickApp 核心                     │
│  ┌───────────┐ ┌───────────┐ ┌────────────────┐ │
│  │ 应用管理   │ │ 页面路由   │ │ 生命周期管理    │ │
│  └───────────┘ └───────────┘ └────────────────┘ │
├─────────────────────────────────────────────────┤
│  ┌───────────┐ ┌───────────┐ ┌────────────────┐ │
│  │ GUI 组件层 │ │ JS Feature│ │   Inspector    │ │
│  │(gui_wrapper)│ │(features) │ │  (调试工具)     │ │
│  └───────────┘ └───────────┘ └────────────────┘ │
├─────────────────────────────────────────────────┤
│                  基础依赖                         │
│  QuickJS · LVGL · Yoga · libuv · protobuf-c     │
└─────────────────────────────────────────────────┘
```

## 代码目录

```tree
├── include/                        # 公开头文件
│   ├── quickapp.h                  # 应用核心 API（创建、路由、生命周期、安装/卸载等）
│   ├── qwrapper.h                  # 平台适配接口（需开发者实现）
│   ├── gui_widget_context.h        # GUI 组件上下文接口
│   ├── quickapp_inspector.h        # Inspector 调试接口
│   └── inspector_dump.h            # 内存与性能诊断接口
├── shell/                          # Shell 集成层源码
│   ├── common/                     # 通用工具（日志、互斥、任务队列、闪屏等）
│   ├── vapp/                       # vapp 独立运行模式入口
│   └── xms/                        # vappxms（XMS 服务集成模式）入口
├── deps/
│   └── js-framework/               # JavaScript 框架层（快应用 JS 运行时）
├── inspector/                      # Inspector 调试工具（CDP 协议实现）
├── modules/                        # 功能模块（路由、动画等）
├── Kconfig                         # NuttX Kconfig 配置项
├── CMakeLists.txt                  # CMake 构建脚本
└── Makefile                        # NuttX Make 构建脚本
```

> **说明：** 核心库 `quickapp`、`gui_wrapper`、`quickappfeatures`、`quickapp_inspector` 以预编译静态库形式提供。

## 预编译产物

本仓库通过 `nuttx_add_prebuilt` 发布以下预编译目标：

| 类型 | 名称 | 说明 |
| --- | --- | --- |
| 静态库 | `quickapp` | 快应用核心框架库 |
| 静态库 | `gui_wrapper` | GUI 组件封装层 |
| 静态库 | `quickappfeatures` | JS Feature 扩展集合 |
| 静态库 | `quickapp_inspector` | CDP 调试工具库 |
| 应用程序 | `vapp` | 独立运行模式，直接运行 RPK 包 |
| 应用程序 | `vappxms` | XMS 服务集成模式，配合系统服务使用 |

## 依赖项

本项目运行于 openvela（基于 NuttX）系统，需要以下组件支持：

| 依赖 | Kconfig 配置 | 说明 |
| --- | --- | --- |
| libuv | `LIBUV` | 跨平台事件驱动库 |
| protobuf-c | `PROTOBUF_C` | Protocol Buffers C 实现 |
| QuickJS | `INTERPRETERS_QUICKJS` | JavaScript 引擎（默认） |
| LVGL | `GRAPHICS_LVGL` | 嵌入式 GUI 库 |
| UIKit | `UIKIT` | UI 组件工具集 |
| Yoga | `LIB_YOGA` | Flexbox 排版引擎 |
| libpng | `LIB_PNG` / `LV_USE_LIBPNG` | PNG 图片解码 |
| FreeType | `LIB_FREETYPE` | 字体渲染（vapp/vappxms 需要） |
| curl | `UTILS_CURL` | 网络请求与图片缓存管理 |
| Feature Framework | `FEATURE_FRAMEWORK` | JS Feature 注册框架 |
| libash | `LIBASH` | 辅助工具库 |

## 使用限制

- 本项目仅支持在 openvela（NuttX）系统上运行，不支持 Linux/macOS 等桌面系统独立构建。
- 核心模块以预编译库形式提供，不包含源码。
- `vappxms` 模式依赖 XMS 系统服务（`SYSTEM_SERVER`），需在支持 XMS 的设备上使用。
- Inspector 调试功能依赖 MQTT 网络通道（`NETUTILS_MQTTC`）。

## 开发指南

### 配置 QuickApp

1. 在 NuttX 的 `menuconfig` 中启用快应用支持：

   ```
   Application Configuration → quick application → Enable quick application
   ```

2. 选择运行模式：
   - 启用 `QUICKAPP_VAPP` 以使用独立运行模式。
   - 启用 `QUICKAPP_VAPP_XMS` 以使用 XMS 服务集成模式。

3. 根据需要调整运行参数：
   - `QUICKAPP_STACKSIZE`：任务栈大小（默认 128KB）。
   - `QUICKAPP_JSHEAPSIZE`：JS 堆大小（默认 4MB）。
   - `QUICKAPP_LOG_LEVEL`：日志级别（0=DEBUG, 5=OFF）。

### 集成 Shell 层

Shell 层是应用入口的集成代码，开发者可基于 `shell/` 目录下的源码进行定制：

- `shell/vapp/`：独立运行模式，适用于直接运行 RPK 包的场景。
- `shell/xms/`：XMS 集成模式，通过 `QuickActivity` / `QuickApplication` 与系统服务交互。
- `shell/common/`：通用组件，包括闪屏、任务队列、权限管理等。

开发者需要实现 `qwrapper.h` 中声明的平台适配接口：

```c
// 获取应用包路径
const char* QWrapperGetAppPath(char* buf, size_t max);

// 获取应用数据目录
const char* QWrapperGetBaseDataDir(const char* package_name, char* buf, size_t max);
```

## 接口说明

核心 API 定义在 `include/quickapp.h` 中，主要接口分类如下。完整的接口参数与返回值说明请参考 [API 参考文档](docs/api-reference.md)。

| 接口类别 | 主要函数 | 说明 |
| --- | --- | --- |
| 应用创建与控制 | `QApplicationCreate`, `QApplicationStart`, `QApplicationRequestExit` | 创建、启动、退出应用实例 |
| 页面路由 | `QApplicationRoute`, `QApplicationStackPagePush`, `QApplicationStackPageBack` | 页面导航与栈管理 |
| 生命周期 | `QApplicationShow`, `QApplicationHide`, `QApplicationBackPress` | 前后台切换与返回键处理 |
| 应用信息 | `QApplicationGetPackageName`, `QApplicationGetManifest` | 获取包名、Manifest 信息 |
| GUI 上下文 | `GUIWidgetContextCreate`, `GUIWidgetInit` | GUI 组件初始化与上下文管理 |
| 调试诊断 | `InspectorDumpAppHeap`, `InspectorDumpJSMem` | 堆快照、内存与 CPU 性能分析 |

## 相关仓

| 仓库 | 说明 |
| --- | --- |
| [open-vela/nuttx](https://github.com/open-vela/nuttx) | openvela NuttX 内核 |
| [open-vela/nuttx-apps](https://github.com/open-vela/nuttx-apps) | openvela NuttX 应用集合 |
