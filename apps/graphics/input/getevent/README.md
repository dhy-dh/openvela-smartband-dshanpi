# Input Event Monitor - getevent

## Features
- 🖱️ Mouse event monitoring (clicks/movement/wheel)
- 👆 Multi-touch event capture (coordinates/pressure/timestamps)
- ⌨️ Keyboard key press detection (key codes/event types)
- 🔍 Automatic input device detection in `/dev`
- ⚙️ Command-line device path specification
- 🚦 Non-blocking I/O with 500ms timeout
- 🔒 Resource-safe memory management

## Build Configuration
Enable in NuttX configuration:
```bash
# Enable getevent utility
CONFIG_GRAPHICS_INPUT_GETEVENT=y

# Optional debug details (requires recompilation)
CONFIG_GRAPHICS_INPUT_GETEVENT_DETAIL_INFO=y
```

## Usage
```bash
# Auto-detect all input devices
getevent

# Monitor specific mouse device
getevent -m /dev/mouse0

# Monitor specific touch device
getevent -t /dev/input0

# Monitor specific keyboard
getevent -k /dev/kbd0

# Combine multiple devices
getevent -m /dev/mouse0 -t /dev/input0
```

## Requirements

### NuttX Input Subsystem
The application is built with NuttX input subsystem support, which is required for input event handling.

### Kernel Syslog Output
The kernel must be configured to output syslog messages for enhanced debugging capabilities.

### Termination
Press `Ctrl+C` to safely terminate the monitoring process.
