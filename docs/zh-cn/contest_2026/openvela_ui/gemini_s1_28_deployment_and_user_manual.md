# OpenVela 智能手环——Gemini S1 2.8 英寸部署与用户使用手册

## 1. 文档说明

本文档说明本项目在 Gemini S1 2.8 英寸开发板上的源码准备、编译、链接、镜像打包、烧录、联网服务部署和用户操作方法。

Gemini 版本是原生 NuttX + LVGL 工程，不是快应用，也不能与 DshanPi 共用构建目录。本文档适用于随项目提供的 Gemini S1 2.8 英寸工程。

### 1.1 适用范围

| 项目 | 本手册适用值 |
| --- | --- |
| 开发板 | Gemini S1 2.8 英寸，板级名 `r528s3-gemini-s1` |
| SoC | Allwinner R528 |
| 显示与触摸 | 320 × 240 横屏、ILI9341 SPI LCD、GT911 触摸 |
| 存储 | 128 MiB NAND |
| 板级配置 | `vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/openvela_ui_native` |
| 打包目标 | `r528s3-gemini-s1` |
| 应用实现 | 原生 NuttX + LVGL |
| 主机环境 | Ubuntu 22.04 x86_64；Ubuntu 20.04 及以上可参考 |

本手册不适用于 DshanPi 主体板或其他 Gemini 配置。最终镜像包含 Gemini 的 128 MiB NAND 分区表和 ILI9341/GT911 驱动，不能烧录到 DshanPi 256 MiB NAND 板。

### 1.2 与主体板的差异

- Gemini 使用独立工程、独立 `defconfig`、独立 `nsh.fex` 和独立 lichee 输出目录。
- UI 保持 432 × 514 设计坐标，在 320 × 240 横屏上按比例缩放并居中；左右留黑属于当前适配设计，不是花屏。
- 当前 Gemini 天气页提供北京、上海、广州、深圳、武汉五个固定城市，不包含主体板后续增加的城市添加/删除页。
- 当前 Gemini 没有主体板的一分钟应用级待机；本版本未接入可用的 BMI160 数据源，不支持抬腕亮屏。
- 运动、健康、音乐和 Wi-Fi 同步的核心交互与主体项目一致。
- 当前 Gemini 镜像的启动链会由 `rcS.nsh` 启动 `mediad`、NTP 和 Wi-Fi，外层 `rcS` 还会再次启动 Wi-Fi。这可能引起重复关联、NTP 早于 DHCP 或音频设备争用，因此网络与音乐稳定性可能低于主体板。

健康数据是软件生成的模拟值，不是医疗测量结果；运动数据同样仅供演示。板上没有定位功能，天气城市手动选择。健康和运动数据通过 Wi-Fi 上传，不使用蓝牙。

## 2. 工程包完整性检查

### 2.1 确认目录

请在 Linux 文件系统中完整解压 Gemini 工程，不要只复制应用子目录，也不要与 DshanPi 工程合并。关键目录如下：

```text
openvela-contest-gemini-s1-28/
├── build.sh
├── apps/
├── external/
├── frameworks/
├── nuttx/
├── packages/
├── prebuilts/
├── tools/weather_proxy/
└── vendor/
```

后续命令统一使用环境变量表示实际解压位置：

```bash
export GEMINI_ROOT="$HOME/openvela-contest-gemini-s1-28"
```

本项目包含定制应用、BSP 和素材，不能用普通官方仓库覆盖 `vendor/` 或 `packages/`，也不要在该工程中执行 `repo sync -d` 或 `git clean -fdx`。

### 2.2 检查工具链

```bash
cd "$GEMINI_ROOT"

ls -ld prebuilts
readlink prebuilts || true
test -x prebuilts/gcc/linux-x86_64/arm-none-eabi/bin/arm-none-eabi-gcc
```

最后一条命令必须成功。如果 `prebuilts` 指向另一台机器的绝对路径，请重新取得完整工程；也可以在确认当前项确实是符号链接后，将它改到本机已有的同版本 openvela 工具链：

```bash
cd "$GEMINI_ROOT"
test -L prebuilts
mv prebuilts prebuilts.invalid-link.backup
ln -s /absolute/path/to/complete-openvela/prebuilts prebuilts
test -x prebuilts/gcc/linux-x86_64/arm-none-eabi/bin/arm-none-eabi-gcc
```

### 2.3 首次构建前清理

```bash
cd "$GEMINI_ROOT"
./build.sh vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/openvela_ui_native -j2 distclean
```

随后按第 6 节重新编译。如果日志仍出现另一台机器的绝对路径，请停止构建并重新解压完整工程，不要继续 pack。

## 3. Ubuntu 编译环境

### 3.1 资源建议

- Ubuntu 22.04 x86_64。
- 至少 8 GiB 内存，建议 16 GiB。
- Gemini 源码与工具链建议预留 40 GiB 以上；若同时保存主体工程，建议总计 100 GiB。
- Bash shell。首次补齐依赖时需要网络。

### 3.2 安装依赖

```bash
sudo apt update
sudo apt install -y \
  bison flex gettext texinfo libncurses5-dev libncursesw5-dev xxd \
  git git-lfs curl cmake gperf automake libtool build-essential genromfs \
  libgmp-dev libmpc-dev libmpfr-dev libisl-dev binutils-dev libelf-dev \
  libexpat1-dev gcc-multilib g++-multilib libc6-i386 picocom \
  u-boot-tools util-linux dfu-util libx11-dev libxext-dev net-tools \
  pkgconf unionfs-fuse zlib1g-dev libusb-1.0-0-dev libv4l-dev \
  libuv1-dev npm nodejs nasm yasm libdivsufsort-dev libc++-dev \
  libc++abi-dev libprotobuf-dev protobuf-compiler protobuf-c-compiler \
  mtools kconfig-frontends python3 python3-pip python-is-python3

git lfs install
python3 -m pip install --user kconfiglib pyelftools cxxfilt
```

`dragon` 打包工具需要 x86_64 Ubuntu 的 32 位兼容库，因此不要省略 `libc6-i386` 和 multilib 包。

## 4. 下载后预检

```bash
export GEMINI_ROOT="$HOME/openvela-contest-gemini-s1-28"
cd "$GEMINI_ROOT"

test -x ./build.sh
test -x nuttx/tools/build.sh
test -x prebuilts/gcc/linux-x86_64/arm-none-eabi/bin/arm-none-eabi-gcc
test -f packages/demos/openvela_ui/openvela_ui_main.c
test -f vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/openvela_ui_native/defconfig
test -f vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/configs/sys_partition.fex
test -x vendor/allwinnertech/lichee/tools/tool/dragon
test -f tools/weather_proxy/server.js
```

任一命令失败都说明目录、权限、符号链接或工程内容不完整。请重新解压完整工程后再检查；如果代理位于单独的共享目录，把最后一条改成该目录的实际路径。

DshanPi 与 Gemini 必须使用两个隔离目录。首次构建建议串行处理并逐个核对。不要从主体工程的终端直接切换 Gemini lunch，也不要把主体的 `nsh.fex`、`data/usrdata` 或 `.img` 复制进 Gemini 输出目录。

## 5. 打包前配置

### 5.1 Wi-Fi

文件：

```text
vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/data/usrdata/etc/wifi/wapi.conf
```

保持下列结构，换成部署现场的 WPA2-Personal 信息：

```json
{
  "wlan0": {
    "mode": 2,
    "auth": 4,
    "cmode": 8,
    "alg": 3,
    "ssid": "YOUR_WIFI_SSID",
    "bssid": "00:00:00:00:00:00",
    "psk": "YOUR_WIFI_PASSWORD"
  }
}
```

密码会明文进入本地生成的镜像，只应填写本人有权使用的网络信息，不要转发含真实密码的源码目录或镜像。

### 5.2 天气

文件：

```text
vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/data/usrdata/etc/openvela_ui/weather.conf
```

```ini
proxy_url=http://YOUR_PUBLIC_HOST/api/weather
```

板端天气客户端当前使用 `http://`。`YOUR_PUBLIC_HOST` 只写主机名；Cloudflare Tunnel 把公网请求回源到本地代理。

### 5.3 健康和运动同步

文件：

```text
vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/data/usrdata/etc/openvela_ui/sync.conf
```

```ini
enabled=1
endpoint=https://YOUR_PUBLIC_HOST/api/sync/frame
control_url=https://YOUR_PUBLIC_HOST/api/sync/control?deviceId=openvela-gemini-s1-01
device_id=openvela-gemini-s1-01
```

Gemini 与主体板共用代理时也必须使用不同 `device_id`。配置在应用启动时读取，修改板端文件后重启应用或整板。

开始 pack 前，请检查这三个配置文件并填入自己的 SSID、PSK、Tunnel 主机和唯一 `device_id`，不要沿用示例或他人的现场配置。QWeather 私钥只保存在电脑端，不能复制进工程或镜像。

### 5.4 素材

实体板素材打包源为：

```text
vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/data/usrdata/openvela_ui/
```

`packages/demos/openvela_ui/deploy_assets.sh` 只用于模拟器/ADB，不属于 Gemini 实体板部署流程。

## 6. 编译与链接

使用当前项目最终配置：

```bash
export GEMINI_ROOT="$HOME/openvela-contest-gemini-s1-28"
cd "$GEMINI_ROOT"

./build.sh vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/openvela_ui_native -j2
```

`-j2` 是 LTO 构建的保守值。链接阶段可能长时间没有新输出，请等待完成，不要改用主体板的 `configs/nsh`。

该命令已经完成编译和链接。POSTBUILD 会对 Gemini 的 NuttX 产物执行板级处理并生成 `nsh.fex`，不要手工把巨大的 `nuttx.bin` 复制成 `nsh.fex`。验证：

```bash
cd "$GEMINI_ROOT"

test -s nuttx/nuttx
test -s nuttx/nuttx.elf
test -s nuttx/nuttx.map
test -s vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/configs/nsh.fex

file nuttx/nuttx
stat vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/configs/nsh.fex
```

`nsh.fex` 应是 POSTBUILD 生成的精简程序，不能用 DshanPi 的比较规则去要求它与 Gemini 的 raw `nuttx.bin` 完全相同。

修改 `defconfig`、切换过配置或上次中断时，先清理再构建：

```bash
cd "$GEMINI_ROOT"
./build.sh vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/openvela_ui_native -j2 distclean
./build.sh vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/openvela_ui_native -j2
```

## 7. 生成可烧录镜像

建议使用新 Bash 终端：

```bash
export GEMINI_ROOT="$HOME/openvela-contest-gemini-s1-28"
cd "$GEMINI_ROOT/vendor/allwinnertech/lichee"

source envsetup.sh
lunch_nuttx r528s3-gemini-s1
pack
```

必须在 lichee 目录 `source envsetup.sh`，随后用非交互参数选择 Gemini。不要使用 DshanPi lunch，也不要直接运行其他 `pack.sh`。

成功判据：

```text
Dragon execute image.cfg SUCCESS !
pack finish
```

最终镜像：

```text
vendor/allwinnertech/lichee/out/r528s3/gemini-s1_nand/rtos_nuttx_r528s3-gemini-s1_uart0_128Mnand.img
```

验证：

```bash
export IMAGE="$GEMINI_ROOT/vendor/allwinnertech/lichee/out/r528s3/gemini-s1_nand/rtos_nuttx_r528s3-gemini-s1_uart0_128Mnand.img"

test -s "$IMAGE"
stat "$IMAGE"
sha256sum "$IMAGE"
```

本手册对应版本的参考镜像大小为 `68690944` 字节；修改源码、素材或打包工具后，大小和 SHA-256 会变化。

只编译不执行 `pack` 得不到完整可烧录镜像，因为字体、PNG、PCM、Wi-Fi、天气和同步配置是在 pack 阶段写入 usrdata 分区的。

## 8. Windows 烧录

### 8.1 准备与警告

- 安装全志 USB 线刷驱动和 PhoenixSuit。
- 选择文件名含 `r528s3-gemini-s1` 与 `128Mnand` 的镜像。
- 备份板上需要保留的数据。

“全盘擦除升级”会删除 `/data` 中的 Wi-Fi、运动目标、历史和同步队列。

### 8.2 PhoenixSuit 操作

1. 打开 `PhoenixSuit.exe`，选择“一键刷机”。
2. 选择第 7 节生成的 Gemini 128 MiB NAND 镜像。
3. 选择“全盘擦除升级”。
4. 按住板上 `FEL`/下载键，同时短按 `RST`，再松开 `FEL`。
5. Windows 识别线刷设备后等待烧写完成和设备重启。

不要选择 DshanPi 文件名含 `256Mnand` 的镜像。烧录中不要断电或拔线。

## 9. 串口与首次启动检查

MobaXterm 新建 Serial 会话：

- Serial port：实际 COM 号。
- Speed：`1500000`。
- Flow Control：`none`。

进入 NSH 后执行：

```sh
ps
mount
ls -l /data/openvela_ui
ls -l /data/etc/openvela_ui
ifconfig
date
ntpcstatus
```

检查要点：

- 预期只有一个 `openvela_ui` Task/进程组；同一 GROUP 下出现一个或多个名为 `openvela_ui` 的 pthread worker 属于正常现象。不要再次手工启动第二个 `openvela_ui` Task、Wi-Fi、NTP 或 `mediad`。
- `/data/openvela_ui` 素材完整。
- `wlan0` 获得实际局域网地址，系统日期合理。
- UI 在 320 × 240 屏幕上按比例居中，留黑不算故障。
- 触摸映射正确，无持续 Data abort、Undefined instruction 或花屏。

Wi-Fi 失败时检查：

```sh
cat /data/etc/wifi/wapi.conf
cat /tmp/resolv.conf
ifconfig
ping YOUR_GATEWAY_IP
```

如果启动脚本确实没有运行，才执行一次：

```sh
sh /etc/wifi/start_wifi.sh
```

不要并发重复启动网络脚本。Gemini 的 NTP 服务器与主体板配置可能不同；判断校时应同时查看 `date`、`ntpcstatus` 和 HTTP Date 后备日志。

当前镜像自身的启动链会从两处启动 Wi-Fi，并启动 `mediad`。如果日志显示重复关联或音乐设备争用，不要再手工追加服务；重启只用于恢复异常状态，重启后的单次启动周期内仍可能看到脚本自身产生的重复 Wi-Fi/NTP 日志。

## 10. 电脑端天气与同步服务

先确认天气代理目录的位置。默认路径为 Gemini 工程内的 `tools/weather_proxy/`；如果下载资料把代理放在共享目录，请把变量改为实际路径：

```bash
export PROXY_ROOT="$GEMINI_ROOT/tools/weather_proxy"
test -f "$PROXY_ROOT/server.js"
```

检查失败时，天气和同步服务无法部署，请确认下载内容完整。

### 10.1 启动本地代理

```bash
cd "$PROXY_ROOT"

export QWEATHER_API_HOST='YOUR_QWEATHER_API_HOST'
export QWEATHER_PROJECT_ID='YOUR_PROJECT_ID'
export QWEATHER_CREDENTIAL_ID='YOUR_CREDENTIAL_ID'
export QWEATHER_PRIVATE_KEY_PATH='/absolute/path/to/ed25519-private.pem'
export WEATHER_SERVER_PORT=8790

node server.js
```

服务只使用 Node.js 内置模块，Node.js 12 或更高版本即可。和风天气私钥必须留在电脑端、工程目录之外，不能进入源码或镜像。

验证：

```bash
curl http://127.0.0.1:8790/health
curl -G http://127.0.0.1:8790/api/weather \
  --data-urlencode 'location=北京' \
  --data-urlencode 'adm=北京'
curl http://127.0.0.1:8790/api/sync/status
```

端口被占用时：

```bash
ss -ltnp | grep ':8790'
```

可停止旧进程，或让 Node 与 Tunnel 一起改用同一个新端口。

### 10.2 Cloudflare Tunnel

`cloudflared` 不由前面的 APT 命令安装。先执行 `command -v cloudflared`；如果没有安装，请按 Cloudflare 官方 Linux x86_64 说明安装。若下载资料另附经过校验的可执行文件，可这样安装和检查：

```bash
sudo install -m 0755 /path/to/cloudflared /usr/local/bin/cloudflared
command -v cloudflared
cloudflared --version
```

板子与代理电脑不在同一 Wi-Fi 时启动：

```bash
cloudflared tunnel --url http://127.0.0.1:8790
```

把输出的 `https://YOUR_RANDOM_NAME.trycloudflare.com` 主机名写回第 5 节配置再重新 pack。Quick Tunnel 每次重启可能改变域名；域名改变后需要再次更新配置和镜像。

当前同步接口没有用户鉴权，只适合比赛演示和模拟数据。不要上传真实健康隐私，也不要把 Quick Tunnel 当作生产服务。

### 10.3 强制 Gemini 上传

```bash
curl -X POST http://127.0.0.1:8790/api/sync/request \
  -H 'Content-Type: application/json' \
  -d '{"deviceId":"openvela-gemini-s1-01"}'
```

等待约 15 秒后检查：

```bash
curl http://127.0.0.1:8790/api/sync/status
curl 'http://127.0.0.1:8790/api/sync/records?deviceId=openvela-gemini-s1-01'
```

板端预期出现 `sync queued ... (simulation)` 和 `sync upload acknowledged`。强制测试记录写入 `tmp/daily-sync-receiver/openvela-gemini-s1-01/simulation/`。系统日期早于 2020 时不会形成正常当日上传，应先校时。

## 11. 用户操作

### 11.1 主干与手势

- 左右循环滑动六个主干页：主页、天气、运动、健康、音乐、通知。
- 主页向下滑进入背景/动作选择，向上滑返回。
- 在天气、运动、健康或音乐主干页向下滑进入对应详情，按界面提示继续左右或上下操作。
- 由于 UI 被缩放到 320 × 240，手势应从有效 UI 区域内部开始。

### 11.2 天气

- 天气依赖 Wi-Fi、合理系统时间、Node 代理和可访问的 Tunnel。
- 当前 Gemini 提供北京、上海、广州、深圳、武汉五个固定城市。
- 横向切换城市或天气内容，纵向返回。
- Gemini 当前版本不提供城市滚轮添加、删除和当前位置功能。

### 11.3 运动

- 显示步数、卡路里和运动时长，可纵向切换指标。
- 横向进入 7 日历史与目标页，并用数字键盘修改目标。
- 目标范围：步数 `1000–99999`、卡路里 `50–9999 kcal`、时长 `5–1440 min`。
- 24 小时图可横向滚动，点击小时坐标显示该小时值；大于等于 `10000` 使用“万”缩写。
- 三个目标全部达到时显示当日完成弹窗。
- 运动模式和计数用于软件演示，不代表专业传感器记录。

### 11.4 健康

- 心率生成 `60–100 BPM` 演示值。
- 血压生成收缩压 `105–139 mmHg`、舒张压 `65–89 mmHg` 演示值并显示脉搏。
- 完成测量后记录才进入历史与 Wi-Fi 同步。
- 这些数据不可用于诊断、治疗、用药或其他医疗判断。

### 11.5 音乐

- 支持播放/暂停、上一首、下一首、播放列表与音量调节。
- 音乐资源是 24 kHz、单声道、S16LE raw PCM，经 NxPlayer 直接写入 `/dev/audio/pcm0p`。
- 不要额外启动第二个媒体服务或第二个 UI 实例争用声卡。
- 当前实板已知播放中直接切歌可能触发 XRUN/-EPIPE 并导致后续静音，推荐先暂停再切歌；异常时重启恢复。

### 11.6 通知

通知当前只是六页主干中的展示/占位页，不提供消息列表、手机消息接入、蓝牙推送或下滑详情。

### 11.7 屏幕与传感器范围

- Gemini 当前不包含主体板的 60 秒应用级息屏策略，本版本不会按该策略自动息屏。
- 当前配置没有可用的 BMI160/抬腕手势数据源，不支持抬腕亮屏。
- I2C Bus 显示存在只表示控制器可用，不表示传感器已焊接、上电或驱动已经输出数据。

如需核实传感器，可执行：

```sh
uorb_listener sensor_accel_uncal,sensor_gyro_uncal -n 10 -t 10
uorb_listener sensor_wake_gesture,sensor_pickup_gesture -n 3 -t 5
```

命令没有输出消息是本版本的预期现象，不代表命令本身执行失败。

## 12. 常见故障

| 现象 | 检查与处理 |
| --- | --- |
| `prebuilts/...arm-none-eabi-gcc` 不存在 | `prebuilts` 可能是另一台机器的绝对断链；按第 2.2 节重新获取完整工程或修正工具链链接。 |
| 编译日志出现另一个用户的绝对路径 | 先按第 2.3 节执行 `distclean`；若仍出现，停止构建并重新解压完整工程。 |
| 编译 OOM 或链接似乎停住 | 用 `-j2`，关闭高内存程序并等待 LTO。 |
| `dragon` 无法执行 | 安装 `libc6-i386` 和 multilib，使用 x86_64 Ubuntu。 |
| pack 后烧录仍是旧代码 | 每次先编译，确认 Gemini `nsh.fex` 时间更新，再 lunch Gemini 并 pack。 |
| 白屏、触摸错位或启动崩溃 | 核对镜像名含 `gemini-s1` 与 `128Mnand`，排除 DshanPi 配置/素材/分区串入。 |
| UI 左右留黑 | 当前 432 × 514 设计按比例缩放到 320 × 240 后居中，是预期适配。 |
| Wi-Fi 反复关联或服务重复 | 当前 Gemini 镜像启动链本身存在两处 Wi-Fi 启动且会启动 `mediad`；不要再手工追加服务，重启只用于恢复状态，重复启动日志仍可能出现。 |
| 天气超时 | 检查 Wi-Fi、DNS、Node、Tunnel、HTTP weather URL 和 Quick Tunnel 域名。 |
| 同步进入 15/30/60 秒退避 | 用 curl 验证 HTTPS control URL、CA 和时间，核对唯一 device ID，改配置后重启。 |
| 同步返回 `-36` | `-36` 表示文件名过长；检查是否烧录了旧同步实现或生成了超长 outbox 临时文件名。 |
| 播放中切歌后静音 | 使用“暂停 → 切歌 → 播放”；异常时重启。 |
| 烧录后用户数据丢失 | 全盘擦除会重建 `/data`，烧录前备份。 |

## 13. 关键路径速查

| 内容 | 路径 |
| --- | --- |
| 应用源码 | `packages/demos/openvela_ui/` |
| Gemini 最终配置 | `vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/openvela_ui_native/` |
| 板级启动脚本 | `vendor/allwinnertech/boards/r528/r528s3-gemini-s1/src/etc/init.d/rcS` |
| 镜像内素材源 | `vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/data/usrdata/openvela_ui/` |
| Wi-Fi 配置源 | `vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/data/usrdata/etc/wifi/wapi.conf` |
| 天气/同步配置源 | `vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/data/usrdata/etc/openvela_ui/` |
| 链接后打包输入 | `vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/configs/nsh.fex` |
| 最终镜像 | `vendor/allwinnertech/lichee/out/r528s3/gemini-s1_nand/rtos_nuttx_r528s3-gemini-s1_uart0_128Mnand.img` |
| 电脑端代理 | `tools/weather_proxy/` 或下载资料指定的共享目录 |
