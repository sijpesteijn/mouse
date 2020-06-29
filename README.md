#To install libusb
sudo apt-get install libusb-dev

use 
```
#include <libusb-1.0/libusb.h>
```

#To run as none root

add a file to /etc/udev/rules/99-usb.rules
These are the usb bus and the usb device
```bash
SUBSYSTEM=="usb", SYSFS{idProduct}=="2510", SYSFS{idVendor}=="093a", MODE="0666"
SUBSYSTEM=="usb", SYSFS{idProduct}=="0002", SYSFS{idVendor}=="1d6b", MODE="0666"
```
Reload rules
```bash
sudo udevadm control --reload
```