# openvela UI native shell

This directory is the isolated native LVGL implementation of the smart-band
reference under `openvela_ui/smart-band-ui-redesign`. The reference project is
read-only: its Quick App source is not compiled into this application.

The native shell implements the six-page circular trunk:

```text
Home <-> Weather <-> Sport <-> Health <-> Music <-> Notifications <-> Home
```

AI Assistant is intentionally excluded. Swipe down on Home to open Appearance,
then choose Background or Action. Swipe up to return. The five background
themes and six cat actions use the reference assets, while the native UI keeps
one shared `lv_animimg` decoder and only builds paths for the selected action.
The Appearance flow is a full-screen overlay, so vertical navigation does not
compete with the horizontal TileView.

The native Sport detail follows the reference interaction chain: the live
Steps/Calories/Duration statistics page swipes left to a seven-day history,
then left again to the current metric goal.  Goals are edited with an on-screen
numeric keypad and stored in `/data/etc/openvela_ui/sport_state.conf`.  When
all three daily goals are reached, one lightweight full-screen celebration is
shown per calendar day.  The history, goal, editor, and celebration object
trees are created once and reused to avoid framebuffer allocation spikes.
The live and historical day views share one horizontally scrollable 00--24
hour cumulative chart.  Hour coordinates are selectable and use one reusable
value bubble; values of at least 10,000 use a one-decimal `万` abbreviation.
State now persists all 24 end-of-hour cumulative samples as `day_hours` and
`history_hours`; legacy 12-sample `day_samples`/`history_samples` records are
expanded monotonically and rewritten on the next save.  The owner can call
`openvela_ui_sport_set_low_power()` to keep accounting cadence unchanged
while suppressing chart refresh and decorative motion.

On the R528 dshanpi target, one minute without touch enters application-level
standby.  The SPI LCD backlight turns off, the clock and decorative Sport/cat
work are reduced, and the first touch is consumed only to wake the screen.
Music playback, weather/network workers, time synchronisation, and persisted
Sport accounting continue.  Standard `sensor_wake_gesture` and
`sensor_pickup_gesture` uORB nodes are detected at runtime and can wake the
display when present.  The current dshanpi/Gemini
configs do not enable an IMU, so they safely fall back to touch wake rather
than treating the LTR553 light/proximity sensor as wrist orientation.

Board-side sensor availability can be checked with:

```sh
ls -l /dev/uorb
uorb_listener sensor_light,sensor_prox -n 10 -t 15
uorb_listener sensor_accel -n 3 -t 5
uorb_listener sensor_wake_gesture,sensor_pickup_gesture -n 3 -t 5
```

Runtime assets are isolated below the configured data root:

```text
backgrounds/  five 432x514 themes
actions/      five additional frame sequences
cat/          default cover-dance sequence
icons/        primary-page markers
music/        player icons and 24 kHz mono S16LE PCM tracks
```

Music playback uses NxPlayer directly with `/dev/audio/pcm0p`; it does not
require `mediad`.  `deploy_assets.sh` converts the reference MP3 tracks to raw
PCM with GStreamer before pushing them, matching the files carried by the
R528 board images.  The host therefore needs `gst-launch-1.0` together with
the `mpegaudioparse` and `mpg123audiodec` plugins.

Enable `CONFIG_LVX_USE_DEMO_OPENVELA_UI`, build the image, start the emulator,
then deploy runtime assets:

```sh
./packages/demos/openvela_ui/deploy_assets.sh /path/to/smart-band-ui-redesign
adb shell 'openvela_ui &'
```
