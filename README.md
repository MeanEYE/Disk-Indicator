Disk-Indicator
==============

Small program for Linux that will turn your `Scroll`, `Caps` or `Num Lock` LED into hard disk indicator.

### Usage

```
sudo ./disk_indicator [-x] [-t]

-x     Use X.Org for detecting keyboard flags. If you are using graphical desktop
       environment use this flag to avoid getting your LEDs reset.

-t     Detect current tty instead of defaulting to /dev/console.
       If you are running this program from tty use this flag. It will enable
       program to operate without super user priviledges.
```
