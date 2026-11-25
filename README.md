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

#### Sharing single LED:
Multiple devices can share the same LED. Add one line per device, all of them with the same `led` value:
```
led=e|scroll event=both device=sda
led=e|scroll event=both device=nvme0n1
```
