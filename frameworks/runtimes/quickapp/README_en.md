# QuickApp Application Container

\[ English | [简体中文](README.md) \]

## Overview

QuickApp Application Container is a [QuickApp](https://doc.quickapp.cn/) runtime framework for the [openvela](https://github.com/open-vela) system, providing a lightweight execution environment for QuickApp on AIoT devices such as smartwatches, smart speakers, and smart panels.

Built on the [QuickJS](https://bellard.org/quickjs/) JavaScript engine, combined with the [LVGL](https://lvgl.io/) graphics library and the [Yoga](https://yogalayout.dev/) layout engine, this project implements core capabilities defined in the QuickApp standard, including page rendering, routing, and lifecycle management.

Key features:

- Loading and running QuickApp RPK packages.
- LVGL-based GUI rendering and widget system.
- Page routing and navigation stack management.
- Application lifecycle management (create, foreground, background, destroy, etc.).
- JavaScript Feature extension mechanism.
- Remote debugging via CDP (Chrome DevTools Protocol) Inspector.

> **Note:** This repository distributes core modules as prebuilt libraries, while exposing header files and Shell integration layer source code for application integration and customization.

## Architecture

```
┌─────────────────────────────────────────────────┐
│              Shell Integration Layer             │
│           (vapp / vappxms entry points)          │
├─────────────────────────────────────────────────┤
│                 QuickApp Core                    │
│  ┌───────────┐ ┌───────────┐ ┌────────────────┐ │
│  │    App     │ │   Page    │ │   Lifecycle    │ │
│  │ Management │ │  Routing  │ │  Management    │ │
│  └───────────┘ └───────────┘ └────────────────┘ │
├─────────────────────────────────────────────────┤
│  ┌───────────┐ ┌───────────┐ ┌────────────────┐ │
│  │    GUI     │ │ JS Feature│ │   Inspector    │ │
│  │(gui_wrapper)│ │(features) │ │  (Debug Tool)  │ │
│  └───────────┘ └───────────┘ └────────────────┘ │
├─────────────────────────────────────────────────┤
│                  Dependencies                    │
│  QuickJS · LVGL · Yoga · libuv · protobuf-c     │
└─────────────────────────────────────────────────┘
```

## Directory Structure

```tree
├── include/                        # Public header files
│   ├── quickapp.h                  # Core API (create, route, lifecycle, etc.)
│   ├── qwrapper.h                  # Platform adaptation interface (to be implemented by developers)
│   ├── gui_widget_context.h        # GUI widget context interface
│   ├── quickapp_inspector.h        # Inspector debugging interface
│   └── inspector_dump.h            # Memory and performance diagnostics interface
├── shell/                          # Shell integration layer source code
│   ├── common/                     # Common utilities (logging, mutex, task queue, splash screen, etc.)
│   ├── vapp/                       # vapp standalone mode entry
│   └── xms/                        # vappxms (XMS service integration mode) entry
├── deps/
│   └── js-framework/               # JavaScript framework layer (QuickApp JS runtime)
├── inspector/                      # Inspector debugging tool (CDP protocol implementation)
├── modules/                        # Feature modules (routing, animation, etc.)
├── Kconfig                         # NuttX Kconfig configuration
├── CMakeLists.txt                  # CMake build script
└── Makefile                        # NuttX Make build script
```

> **Note:** Core libraries `quickapp`, `gui_wrapper`, `quickappfeatures`, and `quickapp_inspector` are provided as prebuilt static libraries.

## Prebuilt Artifacts

This repository publishes the following prebuilt targets via `nuttx_add_prebuilt`:

| Type | Name | Description |
| --- | --- | --- |
| Static Library | `quickapp` | QuickApp core framework library |
| Static Library | `gui_wrapper` | GUI widget wrapper layer |
| Static Library | `quickappfeatures` | JS Feature extension collection |
| Static Library | `quickapp_inspector` | CDP debugging tool library |
| Application | `vapp` | Standalone mode, runs RPK packages directly |
| Application | `vappxms` | XMS service integration mode |

## Dependencies

This project runs on openvela (NuttX-based) and requires the following components:

| Dependency | Kconfig Option | Description |
| --- | --- | --- |
| libuv | `LIBUV` | Cross-platform event-driven library |
| protobuf-c | `PROTOBUF_C` | Protocol Buffers C implementation |
| QuickJS | `INTERPRETERS_QUICKJS` | JavaScript engine (default) |
| LVGL | `GRAPHICS_LVGL` | Embedded GUI library |
| UIKit | `UIKIT` | UI component toolkit |
| Yoga | `LIB_YOGA` | Flexbox layout engine |
| libpng | `LIB_PNG` / `LV_USE_LIBPNG` | PNG image decoding |
| LVGL QRCode | `LV_USE_QRCODE` | QR code widget |
| LVGL Barcode | `LV_USE_BARCODE` | Barcode widget |
| LVGL GIF | `LV_USE_GIF` | GIF image decoding and playback |
| LVGL Snapshot | `LV_USE_SNAPSHOT` | Widget snapshot (used for animations, etc.) |
| FreeType | `LIB_FREETYPE` | Font rendering (required by vapp/vappxms) |
| curl | `UTILS_CURL` | Network requests and image cache management |
| Feature Framework | `FEATURE_FRAMEWORK` | JS Feature registration framework |
| libash | `LIBASH` | Utility library |

## Constraints

- This project only runs on openvela (NuttX). Standalone builds on Linux/macOS are not supported.
- Core modules are provided as prebuilt libraries without source code.
- `vappxms` mode requires the XMS system service (`SYSTEM_SERVER`).
- Inspector debugging requires an MQTT network channel (`NETUTILS_MQTTC`).

## Development Guide

### Configuring QuickApp

1. Enable QuickApp support in NuttX `menuconfig`:

   ```
   Application Configuration → quick application → Enable quick application
   ```

2. Select the runtime mode:
   - Enable `QUICKAPP_VAPP` for standalone mode.
   - Enable `QUICKAPP_VAPP_XMS` for XMS service integration mode.

3. Adjust runtime parameters as needed:
   - `QUICKAPP_STACKSIZE`: Task stack size (default 128KB).
   - `QUICKAPP_JSHEAPSIZE`: JS heap size (default 4MB).
   - `QUICKAPP_LOG_LEVEL`: Log level (0=DEBUG, 5=OFF).

### Integrating the Shell Layer

The Shell layer provides application entry point code. Developers can customize it based on the source code under `shell/`:

- `shell/vapp/`: Standalone mode, suitable for running RPK packages directly.
- `shell/xms/`: XMS integration mode, interacting with system services via `QuickActivity` / `QuickApplication`.
- `shell/common/`: Common components including splash screen, task queue, permission management, etc.

Developers must implement the platform adaptation interfaces declared in `qwrapper.h`:

```c
// Get application package path
const char* QWrapperGetAppPath(char* buf, size_t max);

// Get application data directory
const char* QWrapperGetBaseDataDir(const char* package_name, char* buf, size_t max);
```

## API Reference

Core APIs are defined in `include/quickapp.h`. The following table provides an overview. For detailed parameter and return value descriptions, see the [API Reference Document](docs/api-reference_en.md).

| Category | Key Functions | Description |
| --- | --- | --- |
| App Creation & Control | `QApplicationCreate`, `QApplicationStart`, `QApplicationRequestExit` | Create, start, and exit application instances |
| Page Routing | `QApplicationRoute`, `QApplicationStackPagePush`, `QApplicationStackPageBack` | Page navigation and stack management |
| Lifecycle | `QApplicationShow`, `QApplicationHide`, `QApplicationBackPress` | Foreground/background switching and back key handling |
| App Info | `QApplicationGetPackageName`, `QApplicationGetManifest` | Get package name and manifest information |
| GUI Context | `GUIWidgetContextCreate`, `GUIWidgetInit` | GUI widget initialization and context management |
| Debugging | `InspectorDumpAppHeap`, `InspectorDumpJSMem` | Heap snapshots, memory and CPU profiling |

## Related Repositories

| Repository | Description |
| --- | --- |
| [open-vela/nuttx](https://github.com/open-vela/nuttx) | openvela NuttX Kernel |
| [open-vela/nuttx-apps](https://github.com/open-vela/nuttx-apps) | openvela NuttX Applications |
