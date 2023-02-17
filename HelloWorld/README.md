# Hello World

Simple Hello World Example to demonstrate different console methods.

# USB CDC Console

The USB-CDC method is the easiest way to get a Zephyr console on the nRF52840-Dongle as no soldering is required. 

USB-CDC is enabled by default using the following config options:

```
CONFIG_USB_DEVICE_STACK=y
CONFIG_USB_DEVICE_PRODUCT="Zephyr Console"
CONFIG_UART_LINE_CTRL=y
```

It also requires the app.overlay file to assign the cdc_acm_uart0 to Zephyr. If it is missing, the USB device is likely to enumerate, however  you may encounter a problem with the device and a yellow exclamation mark is shown:

![USB Warning](https://raw.githubusercontent.com/craigpeacock/nRF52840_Examples/main/img/USB-CDC-Error.png)

# UART

To enable console output on the UART, open the prj.conf and ensure the following config options are set to yes. 

```
CONFIG_CONSOLE=y
CONFIG_SERIAL=y
CONFIG_UART_CONSOLE=y
```

and rename/remove app.overlay. Otherwise during compilation the following error may result:

```
uart_console.c:594: undefined reference to `__device_dts_ord_110'
```

The default Zephyr nrf52840dongle_nrf52840 board files has TXD (output) connected to pin 20, and RXD (input) connected to pin 17.

![UART Connections](https://raw.githubusercontent.com/craigpeacock/nRF52840_Examples/main/img/nRF52840_Dongle_Outline.png)

# Segger RTT

Segger RTT (Real Time Transfer) allows console output over the Serial Wire Debug (SWD) to the [J-Link RTT Viewer](https://wiki.segger.com/J-Link_RTT_Viewer) application running on the PC. This requires a 10 pin, 1.27mm programming header to be soldered to the underside of the board.

To enable Segger RTT support, open the prj.conf and ensure the following config options are set to yes. 

```
CONFIG_CONSOLE=y
CONFIG_RTT_CONSOLE=y
CONFIG_USE_SEGGER_RTT=y
```

# nRF5 Bootloader

The nRF52840-Dongle ships from the factory with a bootloader that allows code to be uploaded via the USB port using the nRFConnect Programmer. 

To use the bootloader, your application must reside at 0x1000. By default, applications targeting the nrf52840dongle_nrf52840 board on Zephyr support the bootloader.

If you solder a 1.27mm x 10 programming header on the bottom of the device for use with a SWD programmer, you can disable bootloader support (and write you application to 0x0000) by setting:  

```
CONFIG_BOARD_HAS_NRF5_BOOTLOADER=n
```
