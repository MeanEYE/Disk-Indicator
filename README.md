Disk-Indicator
==============

Small program for Linux that will turn your `Scroll`, `Caps` or `Num Lock` LED or LED on your ThinkPad laptop into hard disk indicator.

### Usage

```
sudo ./disk_indicator <method> [params]

-x     Use X.Org for detecting keyboard flags. If you are using graphical desktop
       environment use this flag to avoid getting your LEDs reset.
-c     Use standard TTY console interface to control LEDs.
-t     Use ACPI to set LEDs on ThinkPad laptops.
-f     Do not fork to background.

X.Org params: [led]
num    Use NumLock.
cap    Use CapsLock.
scr    Use ScrollLock.

TTY console parameters: [device [led]]
tty[1-9]
console       (default)

num    Use NumLock.
cap    Use CapsLock.
scr    Use ScrollLock.

ThinkPad parameters: [led]
0-15
```
