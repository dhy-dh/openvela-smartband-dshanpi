# OpenVela 智能手环——DshanPi 部署与用户使用手册

## 1. 文档说明

本文档说明本项目在百问网 `r528s3-dshanpi` 主体开发板上的源码准备、编译、链接、镜像打包、烧录、联网服务部署和用户操作方法。

项目是基于openvela-ui-redesign快应用项目的C+LVGL原生应用的迁移改编，用户可以根据本手册完成项目部署。

### 1.1 适用范围

| 项目 | 本手册适用值 |
| --- | --- |
| 开发板 | 百问网 `r528s3-dshanpi` |
| SoC | Allwinner R528，双核 Arm Cortex-A7 |
| 显示与触摸 | 3.5 英寸 SPI 显示屏、电容触摸 |
| 存储 | 256 MiB SPI NAND |
| 板级配置 | `vendor/allwinnertech/boards/r528/r528s3-dshanpi/configs/nsh` |
| 打包目标 | `r528s3-dshanpi` |
| 应用实现 | 原生 NuttX + LVGL，不是快应用 |
| 主机环境 | Ubuntu 22.04 x86_64；Ubuntu 20.04 及以上可参考 |

本手册不适用于 Gemini S1、模拟器或其他 R528 板。DshanPi 镜像包含 256 MiB NAND 分区表、DshanPi 屏幕和触摸驱动，不能烧录到 Gemini S1。遗留目录 `configs/openvela_ui_native` 不是本项目主体板的最终配置，也不要使用。

### 1.2 功能边界

当前镜像提供主页、天气、运动、健康、音乐和通知六页循环主干，AI 助手页已移除。健康数据为界面演示生成的模拟值，不是医疗设备测量结果；运动数据同样用于功能演示。板上没有可用的定位数据，天气城市由用户手动选择。健康和运动数据通过 Wi-Fi 上传，不使用蓝牙。

## 2. 工程包完整性检查

### 2.1 确认目录

请完整解压下载的 DshanPi 工程，不要只复制应用子目录，也不要与 Gemini 工程合并。以下路径应位于同一个工程根目录：

```text
openvela-contest/
├── build.sh
├── apps/
├── external/
├── frameworks/
├── nuttx/
├── packages/
├── prebuilts/                    # 运行工具链脚本后生成
├── tools/weather_proxy/
└── vendor/
```

后续命令统一使用环境变量表示实际解压位置：

```bash
export OPENVELA_ROOT="$HOME/openvela-contest"
```

本项目包含定制应用、BSP 和素材，不能用普通官方仓库覆盖 `vendor/` 或 `packages/`。DshanPi 与 Gemini 应保存在不同目录，避免混用配置和镜像。

## 3. Ubuntu 编译环境

### 3.1 资源建议

- x86_64 Ubuntu 22.04；不建议在 WSL、Docker、macOS 或 Arm Linux 上直接构建。
- 至少 8 GiB 内存，建议 16 GiB。
- 至少 100 GiB 可用磁盘空间。
- Bash shell。首次补齐依赖时需要网络。

### 3.2 安装主机依赖

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

打包程序 `dragon` 是 32 位 i386 ELF；`gcc-multilib`、`g++-multilib` 和 `libc6-i386` 用于提供其兼容运行环境。

## 4. 下载后预检

以下命令只检查文件，不修改工程：

```bash
export OPENVELA_ROOT="$HOME/openvela-contest"
cd "$OPENVELA_ROOT"

./tools/bootstrap_openvela_prebuilts.sh
test -x ./build.sh
test -x nuttx/tools/build.sh
test -x prebuilts/gcc/linux-x86_64/arm-none-eabi/bin/arm-none-eabi-gcc
test -f packages/demos/openvela_ui/openvela_ui_main.c
test -f vendor/allwinnertech/boards/r528/r528s3-dshanpi/configs/nsh/defconfig
test -x vendor/allwinnertech/lichee/tools/tool/dragon
test -f tools/weather_proxy/server.js
```

任一命令失败都说明工程文件不完整，或权限、符号链接已损坏。请重新解压完整工程后再检查。正常情况下，根目录 `build.sh` 是指向 `nuttx/tools/build.sh` 的符号链接。

两个板子的工程必须放在两个隔离目录；不得让 DshanPi 与 Gemini 共用同一个 `nuttx/.config`、板级链接、`nsh.fex` 或 lichee 输出目录。建议顺序构建并逐个核对镜像，以免拿错产物。

## 5. 打包前配置

这些配置会写进 NAND 镜像。修改后至少要重新执行 `pack`，推荐按第 6、7 节完整重编并打包。

### 5.1 Wi-Fi

文件：

```text
vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/etc/wifi/wapi.conf
```

使用部署现场的 WPA2-Personal SSID 和密码，保持 JSON 结构不变：

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

Wi-Fi 密码会以明文进入本地生成的镜像，只应填写本人有权使用的网络信息，并妥善保管镜像文件。

### 5.2 天气代理

文件：

```text
vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/etc/openvela_ui/weather.conf
```

格式：

```ini
proxy_url=http://YOUR_PUBLIC_HOST/api/weather
```

`YOUR_PUBLIC_HOST` 只写主机名，不写末尾 `/`。当前板端天气客户端使用 `http://`，不要擅自改为 `https://`；Cloudflare 公网入口负责把请求转到电脑端代理。

### 5.3 健康与运动数据同步

文件：

```text
vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/etc/openvela_ui/sync.conf
```

格式：

```ini
enabled=1
endpoint=https://YOUR_PUBLIC_HOST/api/sync/frame
control_url=https://YOUR_PUBLIC_HOST/api/sync/control?deviceId=openvela-dshanpi-01
device_id=openvela-dshanpi-01
```

同一代理下的每块板必须使用不同的 `device_id`。配置仅在应用启动时读取，修改板上文件后需要重启应用或整板。

完成 Wi-Fi、天气和同步配置后再执行 pack。QWeather 私钥只能留在电脑端 Node 代理，不要写入板端配置或镜像。

### 5.4 素材来源

实体板素材打包源为：

```text
vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/openvela_ui/
```

其中包含字体、背景、动画、图标、天气图标和 24 kHz 单声道 S16LE PCM 音乐。`packages/demos/openvela_ui/deploy_assets.sh` 是模拟器/ADB 辅助脚本，不属于实体板编译或打包流程，不要在这里运行。

## 6. 编译与链接

在工程根目录执行经过实板验证的传统 Make 构建：

```bash
export OPENVELA_ROOT="$HOME/openvela-contest"
cd "$OPENVELA_ROOT"

./build.sh vendor/allwinnertech/boards/r528/r528s3-dshanpi/configs/nsh -j2
```

`-j2` 是 LTO、FFmpeg 和 LVGL 组合下的保守并发值。内存充足时可以提高并发，但首次编译建议先用 `-j2`。最后的全程序 LTO 链接可能数分钟没有新输出，不要因此中断。

该命令已经完成配置、编译和链接，不需要手工调用 `ld` 或另行复制 `nuttx.bin`。链接成功后验证：

```bash
cd "$OPENVELA_ROOT"

test -s nuttx/nuttx
test -s nuttx/nuttx.bin
test -s nuttx/nuttx.map
test -s vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/configs/nsh.fex

cmp -s \
  nuttx/nuttx.bin \
  vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/configs/nsh.fex
```

最后一条命令返回 0，说明 POSTBUILD 已把本次链接结果正确送入打包输入。若失败，不要继续 `pack`，否则可能把旧程序装进新镜像。

修改了 `defconfig`、切换过其他板配置或上一次构建被中断时，可先执行：

```bash
cd "$OPENVELA_ROOT"
./build.sh vendor/allwinnertech/boards/r528/r528s3-dshanpi/configs/nsh -j2 distclean
./build.sh vendor/allwinnertech/boards/r528/r528s3-dshanpi/configs/nsh -j2
```

## 7. 生成可烧录镜像

建议打开一个新 Bash 终端，避免继承另一块板的 lunch 环境：

```bash
export OPENVELA_ROOT="$HOME/openvela-contest"
cd "$OPENVELA_ROOT/vendor/allwinnertech/lichee"

source envsetup.sh
lunch_nuttx r528s3-dshanpi
pack
```

必须使用 `source envsetup.sh`，不能写成 `./envsetup.sh`；`pack` 是加载环境后得到的 shell 函数，不要直接运行其他 `pack.sh`。

打包时，`data/usrdata` 中的素材和配置会制成 YAFFS 数据分区，再与 bootloader、资源和 `nsh.fex` 合并。日志中的下列提示属于正常公共资源回退，不代表失败：

```text
not found .../dshanpi_nand/data/res, try default data
```

真正的成功判据是末尾同时出现类似：

```text
Dragon execute image.cfg SUCCESS !
pack finish
```

最终镜像：

```text
vendor/allwinnertech/lichee/out/r528s3/dshanpi_nand/rtos_nuttx_r528s3-dshanpi_uart0_256Mnand.img
```

检查产物：

```bash
export IMAGE="$OPENVELA_ROOT/vendor/allwinnertech/lichee/out/r528s3/dshanpi_nand/rtos_nuttx_r528s3-dshanpi_uart0_256Mnand.img"

test -s "$IMAGE"
stat "$IMAGE"
sha256sum "$IMAGE"
```

参考版本的镜像大小为 `81248256` 字节；源码、素材或打包工具变化后大小与 SHA-256 会变化，应以本次构建产生的校验值为准。

## 8. Windows 烧录

### 8.1 准备

- 安装全志 USB 线刷驱动和 PhoenixSuit。
- 准备开发板供电/串口线与 USB 烧录线。
- 关闭占用烧录设备的其他工具。
- 备份板上需要保留的数据。

“全盘擦除升级”会清除 `/data` 中的 Wi-Fi、用户添加的自定义城市、运动目标、历史记录和同步队列，无法撤销。北京、上海、广州、深圳、武汉五个基础城市由程序内置，重启后仍会存在。

### 8.2 PhoenixSuit 操作

1. 打开 `PhoenixSuit.exe`，进入“一键刷机”。
2. 选择第 7 节生成的 `rtos_nuttx_r528s3-dshanpi_uart0_256Mnand.img`。
3. 选择“全盘擦除升级”。
4. 按住板上 `FEL` 键，同时短按一次 `RST`，再松开 `FEL`。
5. Windows 设备管理器应识别 `USB Device (VID_1f3a_efe8)`，PhoenixSuit 开始烧写。
6. 等待进度完成并自动重启；如果工具询问是否继续升级，选择 `N`。

烧录中不要断电、拔线或误选 Gemini 的 128 MiB NAND 镜像。

## 9. 串口与首次启动检查

MobaXterm 新建 Serial 会话：

- Serial port：设备管理器中实际 COM 号。
- Speed：`1500000`。
- Flow Control：`none`。

Flow Control 不设为 `none` 时可能只能看日志、无法输入。

进入 NSH 后检查：

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

- `ps` 中应只有一个 `openvela_ui` Task/进程组；同一 GROUP 下出现一个或多个名为 `openvela_ui` 的 pthread worker 属于正常现象。不要再次手工启动第二个 Task。
- `/data/openvela_ui` 中可见字体、背景、图标和音乐素材。
- `wlan0` 已取得实际局域网地址。
- 时间已校到当前年份。`ntpcstatus` 有采样表示 NTP 成功；如果 HTTP Date 后备已校时，采样数仍可能为 0，应结合 `date` 和启动日志判断。
- UI 可显示，触摸可响应，无持续 Data abort、Undefined instruction 或 framebuffer 错误。

Wi-Fi 失败时可检查：

```sh
cat /data/etc/wifi/wapi.conf
cat /tmp/resolv.conf
ifconfig
ping YOUR_GATEWAY_IP
```

只在启动脚本没有运行时执行一次：

```sh
sh /etc/wifi/start_wifi.sh
```

不要并发多次启动 Wi-Fi 脚本。

## 10. 电脑端天气与同步服务

天气和数据同步共用一个 Node.js 服务及同一端口。Node.js 12 或更高版本即可，项目不依赖第三方 npm 包。

### 10.1 启动本地代理

和风天气私钥必须放在工程目录之外：

```bash
export OPENVELA_ROOT="$HOME/openvela-contest"
cd "$OPENVELA_ROOT/tools/weather_proxy"

export QWEATHER_API_HOST='YOUR_QWEATHER_API_HOST'
export QWEATHER_PROJECT_ID='YOUR_PROJECT_ID'
export QWEATHER_CREDENTIAL_ID='YOUR_CREDENTIAL_ID'
export QWEATHER_PRIVATE_KEY_PATH='/absolute/path/to/ed25519-private.pem'
export WEATHER_SERVER_PORT=8790

node server.js
```

私钥不能写入源码、镜像、日志或本手册。新终端验证：

```bash
curl http://127.0.0.1:8790/health
curl -G http://127.0.0.1:8790/api/weather \
  --data-urlencode 'location=北京' \
  --data-urlencode 'adm=北京'
curl http://127.0.0.1:8790/api/sync/status
```

出现 `EADDRINUSE` 时先检查占用者：

```bash
ss -ltnp | grep ':8790'
```

可以结束旧代理后重启，或统一改用其他端口；Node 服务和 Tunnel 必须指向同一个端口。

### 10.2 跨网络公网代理

两块板处于不同 Wi-Fi 时，可使用 Cloudflare Tunnel。先检查 `cloudflared` 是否已经安装；如果没有，请按 Cloudflare 官方 Linux x86_64 安装说明安装。若下载资料另附经过校验的可执行文件，可这样安装：

```bash
sudo install -m 0755 /path/to/cloudflared /usr/local/bin/cloudflared
command -v cloudflared
cloudflared --version
```

确认命令可用后，在代理主机启动：

```bash
cloudflared tunnel --url http://127.0.0.1:8790
```

把输出的 `https://YOUR_RANDOM_NAME.trycloudflare.com` 中主机名写入第 5 节的 `weather.conf` 和 `sync.conf`。Quick Tunnel 每次重启可能换域名；域名改变后需要更新两份配置并重新打包。

当前代理的同步状态、控制和上传接口没有用户鉴权，只适合比赛演示和本项目模拟数据。不要上传真实健康隐私，也不要把该 Quick Tunnel 当作生产服务。

### 10.3 强制上传与查看结果

板端每约 15 秒轮询一次控制请求。在代理机执行：

```bash
curl -X POST http://127.0.0.1:8790/api/sync/request \
  -H 'Content-Type: application/json' \
  -d '{"deviceId":"openvela-dshanpi-01"}'
```

随后检查：

```bash
curl http://127.0.0.1:8790/api/sync/status
curl 'http://127.0.0.1:8790/api/sync/records?deviceId=openvela-dshanpi-01'
```

板端预期出现 `sync queued ... (simulation)` 和 `sync upload acknowledged`。强制测试记录位于 `tmp/daily-sync-receiver/openvela-dshanpi-01/simulation/`；正式日记录使用 `YYYYMMDD.json`，两者不会互相覆盖。有效日期早于 2020 时不会形成正常当日上传，应先解决校时。

## 11. 用户操作

### 11.1 通用手势

- 六个主干页可左右循环滑动：主页、天气、运动、健康、音乐、通知。
- 在支持详情的主干页向下滑进入详情，按页面提示左右或上下切换；向上滑返回。
- 滑动从屏幕中部开始，避免从屏幕边框过快划过。
- 主页向下滑进入外观选择，选择背景或猫咪动作；向上滑回主页。

### 11.2 天气

- 天气依赖 Wi-Fi、正确系统时间、电脑端代理和有效公网入口。
- 城市管理列表可横向翻页；点选一个城市进入详情后，再横向切换该城市的当前天气和预报。切换到另一城市需要先回到城市列表重新选择。
- 城市管理中，北京、上海、广州、深圳、武汉为基础城市，不能删除。
- 使用省、市、区滚轮添加自定义城市，最多保存 12 个；删除自定义城市时会弹出确认框。
- 板上没有 GPS/定位功能，“当前位置”不在本项目范围内。

### 11.3 运动

- 运动详情显示步数、卡路里和时长，纵向切换指标。
- 横向进入 7 日历史和目标页；目标页可用数字键盘修改目标。
- 目标范围：步数 `1000–99999`，卡路里 `50–9999 kcal`，时长 `5–1440 min`。
- 小时趋势图可横向滚动；点击下方小时坐标显示该小时累计值，`10000` 及以上用“万”缩写。
- 三项目标全部完成时，每个自然日只显示一次完成弹窗。
- “进入运动”用于演示运动模式和运动中数据变化，并非传感器级专业运动记录。

### 11.4 健康

- 心率测量生成 `60–100 BPM` 的演示值。
- 血压测量生成收缩压 `105–139 mmHg`、舒张压 `65–89 mmHg` 的演示值并显示脉搏。
- 完成测量后记录才进入健康历史和 Wi-Fi 同步数据。
- 所有数值仅用于软件演示，不可用于诊断、用药或医疗判断。

### 11.5 音乐

- 支持播放/暂停、上一首、下一首、播放列表和音量调节。
- 音频由 NxPlayer 直接播放 `/data/openvela_ui/music/tracks/*.pcm` 到 `/dev/audio/pcm0p`，无需手工启动 `mediad`。
- 为避免底层 PCM 在停止和新建 worker 之间发生 XRUN，建议先暂停，再切换歌曲。
- 如果切歌后持续无声，先退出或重启设备，不要反复快速点击切歌键。

### 11.6 通知

通知当前只是六页主干中的展示/占位页，显示模块标识，但没有消息列表、手机消息接入、蓝牙推送或下滑详情。此页面目前不能接收或查看手机通知。

### 11.7 待机与唤醒

- 60 秒没有触摸后进入应用级待机：关闭背光并降低界面刷新频率。
- 待机不是关机；音乐、天气、校时、同步和运动计数仍可在后台工作。
- 第一次触摸只负责亮屏，不会触发触摸位置下方的按钮。
- 程序会探测标准 uORB 抬腕/拾取手势节点；当前 DshanPi 镜像没有可用 IMU 数据时会安全回退为触摸唤醒。

检查手势传感器数据：

```sh
uorb_listener sensor_wake_gesture,sensor_pickup_gesture -n 3 -t 5
uorb_listener sensor_accel_uncal,sensor_gyro_uncal -n 10 -t 10
```

没有消息表示当前 BSP 没有可用手势源；I2C 总线存在并不等于 BMI160 等器件已经接入或驱动已启用。

## 12. 常见故障

| 现象 | 检查与处理 |
| --- | --- |
| 编译日志出现 `/home/其他用户/...` | 停止当前构建，删除该解压目录并重新获取完整工程，或联系项目提供方；不要继续 pack。 |
| 编译时 OOM 或链接长期卡住 | 使用 `-j2`，关闭其他占内存进程，并等待 LTO 完成。 |
| `dragon` 无法执行 | 安装 `libc6-i386`、`gcc-multilib`、`g++-multilib`，确认主机为 x86_64 Linux。 |
| `pack` 成功但烧录后仍是旧界面 | 编译后先比较 `nuttx.bin` 与 `nsh.fex`；每次严格执行“编译 → lunch → pack”。 |
| 烧录后白屏或触摸错位 | 核对镜像名必须含 `r528s3-dshanpi` 和 `256Mnand`；排除 Gemini 串板构建污染。 |
| 天气请求超时 | 检查 Wi-Fi、DNS、Node 服务、Tunnel、`weather.conf` 和 Quick Tunnel 是否已换域名。 |
| 同步轮询 `-5` 或退避 | 先用板端/电脑端 curl 验证公网 URL，核对网络、DNS、CA、日期和 `sync.conf`；配置改动后重启。 |
| 同步返回 `-36` | `-36` 是文件名过长，通常说明仍在使用旧同步镜像；请用当前源码重新编译、打包并烧录。 |
| 音乐首次有声，直接切歌后无声 | 先暂停再切歌；异常时重启，避免连续快速切换。 |
| `ntpcstatus` 为 0 但日期正确 | 可能由 HTTP Date 后备完成校时；查看 `date` 和启动日志。 |
| 烧录后城市、目标或历史丢失 | “全盘擦除升级”会重建 `/data`，烧录前应备份需要的数据。 |

## 13. 关键路径速查

| 内容 | 路径 |
| --- | --- |
| 应用源码 | `packages/demos/openvela_ui/` |
| 主体板配置 | `vendor/allwinnertech/boards/r528/r528s3-dshanpi/configs/nsh/` |
| 板级启动脚本 | `vendor/allwinnertech/boards/r528/r528s3-dshanpi/src/etc/init.d/rcS` |
| 镜像内素材源 | `vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/openvela_ui/` |
| Wi-Fi 配置源 | `vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/etc/wifi/wapi.conf` |
| 天气/同步配置源 | `vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/etc/openvela_ui/` |
| 链接后打包输入 | `vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/configs/nsh.fex` |
| 最终镜像 | `vendor/allwinnertech/lichee/out/r528s3/dshanpi_nand/rtos_nuttx_r528s3-dshanpi_uart0_256Mnand.img` |
| 电脑端代理 | `tools/weather_proxy/` |
