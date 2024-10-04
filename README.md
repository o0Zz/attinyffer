# ATTinyffer: ATTiny85 USB Sniffer & HID Controller Simulator
This project provides an easy-to-use solution for simulating a USB controller and sniffing the communication process during controller detection. It is particularly useful for reverse engineering closes systems such as gaming consoles and other devices that rely on HID controllers.

## Prerequisite

- Linux machine or Windows with WSL
- Attiny85: https://fr.aliexpress.com/item/1005007310893943.html + https://fr.aliexpress.com/item/1005006248717173.html
- USB ASP https://fr.aliexpress.com/item/1005001658474778.html

## Setup environment (Linux/Windows WSL)

```
sudo apt-get install avrdude gcc-avr
git clone https://github.com/o0zz/attinyffer.git
cd attinyffer
make clean && make
```

## Flash

Plug the USB-ASP
```
sudo make install
```

### WSL setup
On WSL the USB-ASP will not be available in WSL, to forward it, follow below steps
Install https://github.com/dorssel/usbipd-win/releases

In PowerShell
```
usbipd list 
usbipd bind --busid 2-4
usbipd attach --wsl --busid 2-4
```

In Bash
```
lsusb
```

## Update code

In order to simulate a controller, you will need to update below files in the projet:

- To update the "GET DESCRIPTOR Response DEVICE"
`PROGMEM const char usbDescriptorDevice` 

- To update the "GET DESCRIPTOR Response CONFIGURATION"
`PROGMEM const char usbDescriptorConfiguration`

## Read logs

In order to get log you will need to connect your Attiny85 P0 to an UART adapter RX pin (Settings: 115200 8N1)
Plug the Attiny85 wait ~3s, the log should be automatically displayed.

## Why this projet ?

The v-usb library includes a built-in logger, but it requires a physical UART connection. 
Attiny85 don't have physical UART and using softuart cause issues where the AVR device is not recognized by the host during discovery. 
To address this problem, this implementation bufferize the logs and transmits them only after the host has successfully completed the discovery process.

# FAQ

## avrdude: warning: cannot set sck period. please check for usbasp firmware update.
This warning is not critical and the board can flash without it
