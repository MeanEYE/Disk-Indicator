Disk-Indicator
==============

Small program for Linux that will turn your `Scroll`, `Caps` or `Num Lock` LED or LED on your ThinkPad laptop into hard disk indicator.

### Usage

`./disk_indicator <method> [params]`

* `-x` Use X.Org for detecting keyboard flags. If you are using graphical desktop environment use this flag to avoid getting your LEDs reset.
* `-c` Use standard TTY console interface to control LEDs.
* `-t` Use ACPI to set LEDs on ThinkPad laptops.
* `-f` Do not fork to background.

#### X.Org method

This method uses X.Org protocol to detect and toggle keyboard LED flags. This method only modifies state of LEDs and doesn't apply modifiers assigned to LEDs (doesn't turn CapsLock and others on).

`./disk_indicator -x [led]`

* `num` Use NumLock.
* `cap` Use CapsLock.
* `scr` Use ScrollLock.

_Example:_
`./disk_indicator -x cap`

#### TTY console method

This method requires super-user priviledges to access `/dev/console` or `/dev/tty[1-9]`. It uses Linux kernel API to turn keyboard LEDs on or off.

`sudo ./disk_indicator -c [device [led]]`

_Device:_
* `tty[1-9]`
* `console`

_LED:_
* `num` Use NumLock.
* `cap` Use CapsLock.
* `scr` Use ScrollLock.

_Example:_
`sudo ./disk_indicator -c /dev/tty1 num`

#### ThinkPad method

This method uses ACPI to turn ThinkPad LED on or off. It requires super-user priviledges to access `/proc/acpi/ibm/led` file.

`sudo ./disk_indicator -t [led]`

_LED number:_
`0-15`

_Example:_
`sudo ./disk_indicator -t 0`
