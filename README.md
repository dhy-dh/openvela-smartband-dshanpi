# openvela 智能手环原生 UI（DshanPi 主体板）

本目录是面向百问网 `r528s3-dshanpi` 主体板的 openvela 原生应用工程快照。项目以快应用版智能手环界面为视觉与交互参考，将页面、动画、天气、运动、健康、音乐和数据同步迁移为运行在 NuttX/openvela 上的单进程 LVGL 应用。

这不是快应用 RPK，也不依赖 JavaScript 运行时或快应用路由器。应用从 `/etc/init.d/rcS` 自动启动，直接使用 LVGL、libuv、NxPlayer、NuttX 网络与文件系统接口。

> 本 README 只适用于 DshanPi 主体板的 256 MiB NAND 镜像。Gemini S1 2.8 英寸板使用另一份隔离工程和 128 MiB NAND 镜像，两者禁止混编、混包或混烧。

## 1. 适用硬件与软件基线

| 项目 | 本工程范围 |
| --- | --- |
| 目标板 | 百问网 DshanPi，板级目标 `r528s3-dshanpi` |
| SoC | Allwinner R528，双核 Cortex-A7 |
| 显示与触摸 | 3.5 英寸 SPI 显示屏、电容触摸，framebuffer + FT5X06 板级链路 |
| 存储 | 256 MiB SPI NAND |
| 系统 | NuttX/openvela，基线来自 `dev-ai-contest-2026` |
| 应用框架 | LVGL + NuttX framebuffer/input + libuv |
| 构建配置 | `vendor/allwinnertech/boards/r528/r528s3-dshanpi/configs/nsh` |
| 打包目标 | `r528s3-dshanpi` / `dshanpi_nand` |
| 串口 | 1,500,000 baud，Flow Control 为 `none` |

本目录按“可构建源码快照”提供，不是带完整 `.repo` 历史的 manifest checkout。GitHub 仓库不直接提交约 14 GiB 的通用 `prebuilts/`；首次构建前运行随仓库提供的脚本，从 openvela 官方仓库取得本目标使用的 Linux x86_64 工具。不要在此目录执行 `repo sync -d`、`git clean -fdx` 或用上游 vendor 覆盖本项目改动。

## 2. 当前实现范围

主干页面按环形顺序切换：

```text
主页 → 天气 → 运动 → 健康 → 音乐 → 通知 → 主页
```

已实现：

- 六页循环横向滑动，首尾使用克隆页实现视觉连续。
- 主页、天气、健康、音乐等页面的纵向入口和全屏详情层。
- 主题背景、动作选择及兼顾实板内存和 SPI 带宽的动画。
- 天气实时数据和三日预报；五个基础城市不可删除，最多可添加 12 个自定义城市。
- 城市三级滚轮选择、自定义城市持久化、删除确认弹窗。
- 运动步数、热量、时长、心率、24 小时柱状统计、目标设置和完成弹窗。
- 健康心率与血压演示测量、最近记录及按日同步记录。
- NxPlayer 原始 PCM 音乐播放、暂停、音量和曲目列表。
- Wi-Fi 天气代理、NTP/HTTP Date 校时、健康与运动按日上传。
- 60 秒无触摸后的应用级待机；首个触摸只负责唤醒。
- 可选的标准 uORB 抬腕/拿起手势 topic 探测。

功能边界：

- AI 助手页已从原快应用七页主干中移除。
- 通知页目前是视觉入口，不包含手机通知桥、消息列表或蓝牙推送。
- 电量图标为界面展示，未接入真实电池计量和充电页。
- 板端无 GPS/定位，天气不提供“当前位置”。
- 当前无可用 IMU 数据源时只能触摸唤醒；代码不会从原始加速度自行推断抬腕。
- 步数、心率和血压默认是演示数据。健康数值仅用于 UI 和同步流程演示，不可用于医疗判断。
- 原快应用“连续 3 次达到高心率阈值”的判定与告警行为均未实现，当前页面只有提示文字。

## 3. 与快应用参考工程的区别

快应用参考工程使用 `app.ux`、`manifest.json`、`src/pages/**` 和 `@system.*` 能力。原生版本保留视觉层级和主要交互意图，但运行模型已经改变。

| 快应用实现 | 本原生实现 |
| --- | --- |
| JavaScript/UX 组件和 RPK | C 语言、NuttX 可执行文件和完整 `.img` |
| `manifest.json` 声明路由 | 一个 `lv_tileview` 管理六个主干页 |
| `router.push()` 创建/销毁页面 | 主干 tile 常驻；详情 overlay 进入时创建、正常退出时销毁 |
| DOM 触摸事件和页面生命周期 | LVGL input event、位移判定和 `lv_async_call()` |
| `app.ux` 保存跨页全局状态 | `openvela_ui.c` 中的全局 UI context 和模块状态机 |
| `@system.audio` | NxPlayer 直接输出 `/dev/audio/pcm0p` |
| `@system.storage` | 部分状态写入 `/data`；同步关键文件使用 tmp + fsync + rename |
| `@system.geolocation` | 固定基础城市 + 用户手动添加城市 |
| interconnect/HTTP/蓝牙设想 | 当前只实现 Wi-Fi + libcurl/HTTP(S) |
| ImageAnimator/组件动画 | LVGL animimg、timer 和属性动画 |
| 七个主干页，包含 AI | 六个主干页，AI 已移除 |

### 3.1 迁移时发生的核心逻辑变化

1. **路由改为常驻主干和按需详情。** 六个主干 tile 常驻，详情 overlay 在进入时创建、正常退出时销毁；页面导航由 tile 索引和 overlay 状态控制。
2. **环形切页不复制业务状态。** tileview 内部包含六个真实页和首尾两个克隆页；滑到克隆页后无动画跳回对应真实页，因此状态只有一份。
3. **纵向页面不再是独立路由。** 触摸释放时根据 `delta_x/delta_y` 判定方向，再通过 `lv_async_call()` 延后切换，避免在输入回调中删除当前对象导致悬空访问。
4. **统一逻辑坐标。** UI 以 432 × 514 为设计坐标，通过 `scale_1000` 等比缩放并居中；业务代码不直接依赖某块屏幕的像素尺寸。
5. **网络线程与 UI 线程隔离。** 天气 worker 只更新不可变快照，LVGL 线程按 `revision` 复制结果；后发请求会取消旧 socket，旧城市响应不能覆盖新选择。
6. **动画按实板带宽重构。** 猫动画共享一个解码器，图片缓存统一管理；天气等重图片在入场结束后再显示；全屏切换层保持不透明，减少单 framebuffer 上的花屏和撕裂。
7. **运动页面变为会话状态机。** 正常退出时先写入 `sport_state.conf`，再销毁 overlay 与运动对象树；只有专项运动临时返回主页时会隐藏并保留当前对象。模块通过事件回调请求主界面切换。
8. **音频改为可直接送硬件的 PCM。** MP3 在构建素材阶段转换为 S16LE、24 kHz、单声道 PCM，时长按文件大小推算，不依赖板端 MP3 浮点解码器。
9. **第三方密钥移到电脑代理。** 板端只保存代理地址，QWeather JWT 和 Ed25519 私钥始终在代理机上生成和使用。
10. **数据同步改为可恢复协议。** 健康和运动按日期生成 schema v1 数据，通过 begin/chunk/commit 分帧上传，使用 FNV-1a 校验；仅收到 `ok` 或 `duplicate` 后删除 outbox。

## 4. 运行架构

```mermaid
flowchart TD
    R[rcS] --> W[Wi-Fi / DHCP]
    W --> N[NTP]
    R --> M[openvela_ui_main]
    M --> L[LVGL + libuv UI 线程]
    L --> U[六页 tileview 与详情 overlay]
    L --> S[运动状态机]
    L --> P[待机与唤醒]
    M --> T[HTTP Date 后备校时]
    M --> Y[天气 worker]
    M --> D[同步 worker]
    L --> A[NxPlayer worker]
    Y --> H[电脑端天气/同步代理]
    D --> H
    A --> C[/dev/audio/pcm0p]
    U --> F[/data 资源与状态]
```

启动顺序位于 `openvela_ui_main.c`：

```text
lv_init
→ lv_nuttx_init
→ openvela_ui_create
→ openvela_ui_power_init
→ openvela_ui_timesync_start
→ openvela_ui_sync_start
→ LVGL/libuv event loop
```

退出时按相反方向停止同步、电源管理并释放 LVGL/NuttX 资源。所有 LVGL 对象操作必须发生在 UI 线程；网络、同步或传感器线程不能直接修改控件。

## 5. 代码目录

| 路径 | 作用 |
| --- | --- |
| `packages/demos/openvela_ui/openvela_ui_main.c` | 进程入口、LVGL/NuttX 初始化、后台服务生命周期 |
| `packages/demos/openvela_ui/openvela_ui.c` | 主干 UI、主题、手势、动画及天气/健康/音乐详情 |
| `packages/demos/openvela_ui/openvela_ui_sport.c` | 运动页面、目标、历史、专项运动状态机 |
| `packages/demos/openvela_ui/openvela_ui_weather.c` | 单 worker 天气请求和不可变快照 |
| `packages/demos/openvela_ui/openvela_ui_sync.c` | 健康/运动持久化、控制轮询、分块上传 |
| `packages/demos/openvela_ui/openvela_ui_power.c` | 60 秒待机、触摸/可选 uORB 唤醒、低频刷新 |
| `packages/demos/openvela_ui/openvela_ui_timesync.c` | NTP 未成功时的 HTTP Date 后备 |
| `packages/demos/openvela_ui/openvela_ui_city_data.c` | 主体板城市三级选择数据 |
| `tools/weather_proxy/` | Node.js 天气代理和健康/运动同步接收器 |
| `vendor/allwinnertech/boards/r528/r528s3-dshanpi/` | DshanPi 板级配置 |
| `vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/` | DshanPi 打包配置和 `/data` 素材源 |
| `release/8.3/` | 已验证的 DshanPi 8.3 镜像及校验文件 |

快应用参考与原生代码的主要映射如下：

| 快应用位置/能力 | 原生承接位置 |
| --- | --- |
| `src/app.ux` 全局状态 | `openvela_ui.c` UI context + 各 C 模块状态 |
| `src/manifest.json` 路由 | tile 索引、overlay 枚举和手势分发 |
| `src/pages/**` | `openvela_ui.c` 页面构造函数及 `openvela_ui_sport.c` |
| `src/common/**` 服务 | weather/sync/timesync/power 模块 |
| 图片、字体、音乐 | 打包目录的 `data/usrdata/openvela_ui/` |

## 6. 模块接口与对接方式

### 6.1 UI 生命周期

```c
void openvela_ui_create(void);
void openvela_ui_set_low_power(bool enabled);
```

`openvela_ui_create()` 在 LVGL display/input 初始化后调用一次。不要启动第二个 `openvela_ui` 进程，也不要从 worker 线程重复创建页面。低功耗状态由 power 模块回调到 `openvela_ui_set_low_power()`，用于降低时钟和装饰动画的唤醒频率。

### 6.2 天气接口

```c
int openvela_ui_weather_start(void);
int openvela_ui_weather_request(const char *location_id,
                                const char *location,
                                const char *administrative_area);
int openvela_ui_weather_snapshot(
    struct openvela_ui_weather_snapshot_s *snapshot);
```

对接流程：

1. 应用启动单一网络 worker。
2. UI 选择城市后提交 ID、名称和上级行政区。
3. worker 读取 `/data/etc/openvela_ui/weather.conf`，向电脑代理发起 IPv4 HTTP/1.0 请求。
4. worker 解析 JSON 并发布带新 `revision` 的快照，不访问 LVGL。
5. UI 线程定时复制快照并更新控件；错误约 15 秒后重试，正常数据约 5 分钟刷新。

板端 `weather.conf` 必须使用 `http://`：

```ini
proxy_url=http://YOUR_PUBLIC_HOST/api/weather
```

QWeather 私钥不属于板端接口，必须只通过电脑代理的 `QWEATHER_PRIVATE_KEY_PATH` 提供。

### 6.3 运动与真实传感器接口

运动模块通过 opaque handle 管理完整页面树。常用接口包括：

```c
openvela_ui_sport_t *openvela_ui_sport_create(...);
void openvela_ui_sport_shown(openvela_ui_sport_t *sport);
void openvela_ui_sport_hidden(openvela_ui_sport_t *sport);
bool openvela_ui_sport_gesture(openvela_ui_sport_t *sport,
                               int32_t delta_x, int32_t delta_y);
void openvela_ui_sport_set_data(openvela_ui_sport_t *sport,
                                uint32_t steps, uint16_t heart_rate);
void openvela_ui_sport_set_event_cb(...);
```

`openvela_ui_sport_set_data()` 是接入真实计步/心率源的预留入口。若数据来自驱动线程，应先复制到线程安全缓冲，再由 LVGL timer 或 `lv_async_call()` 在 UI 线程调用该接口。参数为 0 时保留原值。默认工程没有真实传感器生产者。

运动事件由模块通知 owner：关闭详情、进入专项运动、请求主页、确认结束。owner 负责主干页面切换，运动模块不直接删除顶层页面。

### 6.4 健康与同步接口

```c
int openvela_ui_sync_start(void);
void openvela_ui_sync_stop(void);
void openvela_ui_sync_record_heart_rate(uint16_t bpm);
void openvela_ui_sync_record_blood_pressure(uint16_t systolic,
                                             uint16_t diastolic,
                                             uint16_t pulse);
```

只有一次演示测量完整结束后才调用记录接口。同步 worker 等系统日期有效、DNS 可用后工作，读取 `sport_state.conf` 和健康日文件，生成 outbox 并轮询服务端控制请求。当前传输只走 Wi-Fi，不包含蓝牙 GATT 链路。

主要持久化位置：

```text
/data/etc/openvela_ui/cities.conf
/data/etc/openvela_ui/sport_state.conf
/data/etc/openvela_ui/sync/health/YYYYMMDD.conf
/data/etc/openvela_ui/sync/outbox/*.json
/data/etc/openvela_ui/sync/state.conf
```

### 6.5 待机与唤醒接口

```c
void openvela_ui_power_init(lv_display_t *display, lv_indev_t *indev);
void openvela_ui_power_deinit(void);
```

60 秒无触摸后，模块显示顶层黑幕并尝试关闭 framebuffer/backlight；同时把时钟更新从 1 秒降到 60 秒，暂停猫动画和部分装饰刷新。天气、同步、校时和音乐仍继续；运动只在其 overlay 尚存在或专项运动仍激活时继续计时。因此这是应用级待机，不是整机深度睡眠。

模块只探测标准 uORB `sensor_wake_gesture0` 和 `sensor_pickup_gesture0`。没有消息时自动回退为触摸唤醒。首个触摸事件被消费，避免亮屏同时误触按钮。

### 6.6 音频接口

曲目位于 `/data/openvela_ui/music/tracks/track-*.pcm`，格式为：

```text
signed 16-bit little-endian / 24000 Hz / mono
```

NxPlayer 直接占用 `/dev/audio/pcm0p`。播放状态和 40 ms service timer 属于全局 UI context，因此关闭音乐详情后状态仍可保持，重新进入时能恢复显示。暂停和恢复带渐弱/渐强处理。

当前底层仍有异步 stop/new worker 竞争：播放中直接切歌可能出现 XRUN、`-EPIPE` 或后续无声。可靠操作顺序是“暂停 → 切歌 → 播放”；异常时重启板子恢复音频设备。

## 7. 网络代理与配置

源码树中的配置已使用占位值。构建前编辑 DshanPi 自己的打包目录，不要修改 Gemini 路径：

```text
vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/etc/wifi/wapi.conf
vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/etc/openvela_ui/weather.conf
vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/data/usrdata/etc/openvela_ui/sync.conf
```

Wi-Fi 使用 WPA2-Personal JSON 配置：

```json
{"wlan0":{"mode":2,"auth":4,"cmode":8,"alg":3,"ssid":"YOUR_WIFI_SSID","bssid":"00:00:00:00:00:00","psk":"YOUR_WIFI_PASSWORD"}}
```

同步配置示例：

```ini
enabled=1
endpoint=https://YOUR_PUBLIC_HOST/api/sync/frame
control_url=https://YOUR_PUBLIC_HOST/api/sync/control?deviceId=openvela-dshanpi-01
device_id=openvela-dshanpi-01
```

每块板必须使用唯一 `device_id`。Wi-Fi 密码会明文进入镜像，只应在可信环境中填写。

电脑代理要求 Node.js 12 或更高版本：

```bash
cd tools/weather_proxy
QWEATHER_API_HOST='YOUR_QWEATHER_HOST' \
QWEATHER_PROJECT_ID='YOUR_PROJECT_ID' \
QWEATHER_CREDENTIAL_ID='YOUR_CREDENTIAL_ID' \
QWEATHER_PRIVATE_KEY_PATH='/absolute/path/to/ed25519-private.pem' \
WEATHER_SERVER_PORT=8790 \
node server.js
```

本地检查：

```bash
curl http://127.0.0.1:8790/health
curl -G http://127.0.0.1:8790/api/weather \
  --data-urlencode 'location=北京' \
  --data-urlencode 'adm=北京'
curl http://127.0.0.1:8790/api/sync/status
```

若两块板处于不同网络，可用 Cloudflare Tunnel 暴露同一个代理：

```bash
command -v cloudflared
cloudflared tunnel --url http://127.0.0.1:8790
```

Quick Tunnel 重启后域名通常变化，变化后需要更新 `weather.conf` 和 `sync.conf` 并重新 pack。当前同步接收端没有验证可选 token，公网接口只适合比赛演示模拟数据，不要上传真实健康隐私或作为生产服务。

## 8. 编译、链接与打包

推荐 Ubuntu 22.04 x86_64、Bash、8–16 GiB RAM 和至少 100 GiB 可用空间。详细依赖列表、Windows 烧录和串口步骤见[部署与用户使用手册](docs/zh-cn/contest_2026/openvela_ui/dshanpi_deployment_and_user_manual.md)。

预检：

```bash
cd /path/to/openvela-contest
./tools/bootstrap_openvela_prebuilts.sh
test -x build.sh
test -x prebuilts/gcc/linux-x86_64/arm-none-eabi/bin/arm-none-eabi-gcc
test -f packages/demos/openvela_ui/openvela_ui_main.c
test -f vendor/allwinnertech/boards/r528/r528s3-dshanpi/configs/nsh/defconfig
```

首次编译、链接：

```bash
cd /path/to/openvela-contest
./build.sh vendor/allwinnertech/boards/r528/r528s3-dshanpi/configs/nsh -j2
```

`-j2` 是为 LTO、FFmpeg 和 LVGL 内存占用保留余量的建议值。成功后检查：

```bash
test -s nuttx/nuttx
test -s nuttx/nuttx.bin
cmp -s nuttx/nuttx.bin \
  vendor/allwinnertech/lichee/board/r528s3/dshanpi_nand/configs/nsh.fex
```

打包必须在 `lichee` 目录 source 环境后执行：

```bash
cd /path/to/openvela-contest/vendor/allwinnertech/lichee
source envsetup.sh
lunch_nuttx r528s3-dshanpi
pack
```

成功标志包含：

```text
Dragon execute image.cfg SUCCESS
pack finish
```

输出镜像：

```text
vendor/allwinnertech/lichee/out/r528s3/dshanpi_nand/
rtos_nuttx_r528s3-dshanpi_uart0_256Mnand.img
```

不要复用另一个板的 `lunch` 环境、`nsh.fex` 或 `out`。两块板即使分别位于隔离目录，也建议顺序构建并逐个核对镜像名。

## 9. 已验证 8.3 镜像

本目录包含已验证主体板镜像：

- [openvela-ui-dshanpi-8.3.img](release/8.3/openvela-ui-dshanpi-8.3.img)
- [SHA256SUMS](release/8.3/SHA256SUMS)
- 大小：`81,248,256` 字节
- SHA-256：`51383e30ed3b3f0b5e5f8ec35006586827d416589840fab057eeb01179954e7c`

校验命令：

```bash
cd release/8.3
sha256sum -c SHA256SUMS
```

该镜像是原实板验证版本。源码目录中的可重打包配置已使用占位值；网络环境不同时，请填写自己的参数后重新编译/pack。此镜像只能烧录 DshanPi 256 MiB NAND 板。

## 10. 首次启动检查

系统会从 `rcS` 自动启动一个 `openvela_ui` 实例。串口进入 NSH 后可检查：

```sh
ps
mount
ls -l /data/openvela_ui
ls -l /data/etc/openvela_ui
ifconfig
date
ntpcstatus
```

`ntpcstatus` 样本为 0 但日期正确时，可能是 HTTP Date 后备已经完成校时。不要手工再次启动 UI、Wi-Fi、NTP 或音频服务；重复实例会争用 framebuffer、输入或 PCM。

全盘擦除烧录会清除 `/data` 中的 Wi-Fi、城市、运动目标、历史记录和同步队列。具体 PhoenixSuit/FEL 步骤见部署手册。

## 11. 已知限制

- 播放中直接切歌可能出现 XRUN/静音，建议先暂停。
- 无真实 GPS、BMI160/IMU、计步器、心率或血压传感器数据源。
- 通知页和电池信息目前是展示功能。
- 自定义主题、动作选择及 UI 最近健康记录主要保存在 RAM，重启后会恢复默认；完成测量的同步日文件会持久化。
- 待机仅降低 UI 唤醒频率并关闭显示；后台网络与音频仍可运行，运动计时仅在运动 overlay 尚存在或专项运动激活时继续。
- 临时 Tunnel 停止后，本地 UI 仍能使用，但天气刷新和数据上传会进入重试退避。

## 12. 进一步文档

- [DshanPi 部署与用户使用手册](docs/zh-cn/contest_2026/openvela_ui/dshanpi_deployment_and_user_manual.md)
- [应用模块说明](packages/demos/openvela_ui/README.md)
- [电脑端天气/同步代理](tools/weather_proxy/README.md)
