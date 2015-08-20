Disk-Indicator
==============

Small program for Linux that will turn your `Scroll`, `Caps` or `Num Lock` LED or LED on your ThinkPad laptop into hard disk indicator.

### Usage

`./disk_indicator [-f] [-c config.file]`

`-c` Load specified config. (default: ~/.disk-indicator)\n"
`-f` Do not fork to background.\n\n"

### Sample config file:
```
led=t|0 event=read device=sda
led=c|tty1|caps event=write device=sda
led=x|scroll event=both device=sda1
```

#### Config params:
- `led=<provider>|<name>`
	- Provider: Thinkpad (`t`), Console (`c`), XOrg (`x`)
	- Name: `0-15` (only Thinkpad provider), `caps`, `scroll`, `num`
- `event=<type>` - Type: `read`, `write`, `both`
- `device=<name>` - Name: eg. `sda1`, `sda`, `mmcblkp1`
