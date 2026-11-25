Disk-Indicator
==============

Small program for Linux that will turn your `Scroll`, `Caps` or `Num Lock` LED or LED on your ThinkPad laptop into hard disk indicator.

### Usage

`./disk_indicator [-f] [-c config.file]`

- `-c` Load specified config. (default: ~/.disk-indicator)
- `-f` Do not fork to background.

### Sample config file:
```
led=t|0 event=read device=sda
led=c|tty1|caps event=write device=sda
led=x|scroll event=both device=sda1
led=e|scroll event=both device=sda1
led=e|/dev/input/by-id/usb-My_Keyboard-event-kbd|caps event=both device=sda1
```

#### Config params:
- `led=<provider>|<name>`
	- Provider: Thinkpad (`t`), Console (`c`), XOrg (`x`), Input events (`e`)
	- Name: `0-15` (only Thinkpad provider), `caps`, `scroll`, `num`
	- Input events provider works regardless of display server and is the one to use on Wayland. It requires root or membership in `input` group. Without device, `led=e|<name>`, all keyboards with that LED are used. With `led=e|<device>|<name>` only specified one is, where device is either full path or name in `/dev/input`.
- `event=<type>` - Type: `read`, `write`, `both`
- `device=<name>` - Name: eg. `sda1`, `sda`, `mmcblk0p1`
	- Name is the one kernel uses for disk or partition, without `/dev/`. You can get it from `lsblk` or similar tool.
	- Names program can see are in third column of `/proc/diskstats`.

#### Sharing single LED:
Multiple devices can share the same LED. Add one line per device, all of them with the same `led` value:
```
led=e|scroll event=both device=sda
led=e|scroll event=both device=nvme0n1
```

### Providers
Providers are selected at build time with `./configure.sh`. If you are not sure which one to use, input events provider is the best choice for keyboard LEDs.

#### Input events (`e`, `--evdev`)
- Pros:
	- Works the same on Wayland, X.Org and text console.
	- Can use all keyboards at once or a single specified one.
	- No additional libraries needed.
- Cons:
	- Requires root or membership in `input` group. Keep in mind members of that group can read all keyboard input.
	- Only keyboard LEDs are supported.

Finding devices:
- Config is `led=e|<name>` for all keyboards or `led=e|<device>|<name>` for a single one.
- List keyboards with `ls /dev/input/by-id/ | grep event-kbd`. These names don't change between boots, so use full path in config, eg. `led=e|/dev/input/by-id/usb-My_Keyboard-event-kbd|scroll`.
- Built-in laptop keyboards are usually not listed there, look in `/dev/input/by-path/` instead.
- `cat /proc/bus/input/devices` shows all input devices. Ones with `B: LED=` line have LEDs, and `H: Handlers=` line holds short name like `event3`. Short names work as device too, but can change between boots.

#### X.Org (`x`, `--xorg`)
- Pros:
	- Doesn't require root, only access to the display.
- Cons:
	- Works only in X.Org session, not on Wayland or text console.
	- Requires `libx11-dev` to build.
	- Changes to `caps` and `num` LEDs can be ignored by display server, `scroll` is the reliable one.
	- No way to select a single keyboard.

Finding devices:
- Config is `led=x|<name>`. There is no device to select, display from `DISPLAY` environment variable is used and LED is changed on all keyboards.
- Program has to be started from within graphical session, or with `DISPLAY` set and access to it allowed.

#### Console (`c`, `--console`)
- Pros:
	- Doesn't need display server, made for machines used through text console.
	- No additional libraries needed.
- Cons:
	- Requires root.
	- LEDs are tied to the specified console and are not meant to be used in graphical session.
	- No way to select a single keyboard.
	- LEDs stop following lock keys on that console once the program sets them.

Finding devices:
- Config is `led=c|<tty>|<name>`, where tty is name of the console in `/dev`, eg. `led=c|tty1|caps`.
- Run `tty` in text console to get the one you are on, it will print something like `/dev/tty1`.
- `who` lists consoles with logged in users, and `sudo fgconsole` prints number of currently active one.

#### ThinkPad (`t`, `--thinkpad`)
- Pros:
	- Uses LEDs on the laptop itself, leaving keyboard ones alone.
	- No additional libraries needed.
- Cons:
	- Works only on ThinkPad laptops with `thinkpad_acpi` kernel module loaded.
	- Requires root.
	- Which LEDs can be controlled depends on the model.

Finding devices:
- Config is `led=t|<number>`, where number is `0` to `15`.
- `cat /proc/acpi/ibm/led` shows if interface is available. If file is missing, `thinkpad_acpi` module is not loaded.
- Common numbers are `0` for power, `1` and `2` for battery, `7` for standby and `12` for ThinkVantage. Full list is in ThinkPad ACPI section of kernel documentation.
- Meaning of numbers depends on the model, so test before adding to config with `echo "0 off" | sudo tee /proc/acpi/ibm/led` and `echo "0 on" | sudo tee /proc/acpi/ibm/led`.
