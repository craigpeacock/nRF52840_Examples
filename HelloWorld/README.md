# Hello World

Simple Hello World Example to demonstrate different console methods.

# Segger RTT

Segger RTT (Real Time Transfer) allows console output over the Serial Wire Debug (SWD) to the [J-Link RTT Viewer](https://wiki.segger.com/J-Link_RTT_Viewer) application running on the PC.

To enable Segger RTT support, open the prj.conf and ensure the following config options are set to yes. 

```
CONFIG_CONSOLE=y
CONFIG_RTT_CONSOLE=y
CONFIG_USE_SEGGER_RTT=y
```

# UART

To enable console output on the UART, open the prj.conf and ensure the following config options are set to yes. 

```
CONFIG_CONSOLE=y
CONFIG_SERIAL=y
CONFIG_UART_CONSOLE=y
```

The default Zephyr nrf52840dongle_nrf52840 board files has TXD (output) connected to pin 20, and RXD (input) connected to pin 17.

![UART Connections](https://raw.githubusercontent.com/craigpeacock/nRF52840_Examples/main/img/nRF52840_Dongle_Outline.png)


# USB CDC Console







# nRF5 Bootloader

The nRF52840-Dongle ships from the factory with a bootloader that allows code to be uploaded via the USB port using the nRFConnect Programmer. 

To use the bootloader, your application must reside at 0x1000. By default, applications targeting the nrf52840dongle_nrf52840 board on Zephyr support the bootloader.

If you solder a 1.27mm x 10 programming header on the bottom of the device for use with a SWD programmer, you can disable bootloader support (and write you application to 0x0000) by setting:  

```
CONFIG_BOARD_HAS_NRF5_BOOTLOADER=n
```
