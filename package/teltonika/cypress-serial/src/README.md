The sole reason this driver was written - the Cypress CY7C65213 chip, and probably other ones, are unable to perform RTS/CTS flow control in the factory-default USB CDC class mode.

This driver reprograms the chip into vendor class mode and sends proprietary USB packets to control its operation.

One potentially negative side-effect of using this driver: all CY7C65213 chips and similar are (reversibly) reprogrammed once detected. Meaning, that if a USB to Serial adapter, based on a CY7C65213 chip or similar is plugged in, it will be reprogrammed into vendor mode - making it unusable on other Linux machines, unless their kernel supports the chip in vendor class mode (mainline kernel doesn't), or runs this driver.

Therefore, it might be necessary to revert to the CDC class mode:
```
rmmod cypress-serial
insmod cypress-serial cdc=y
```
The `cdc=y` argument instructs to reprogram any detected chips to the CDC class mode. Bear in mind that after rebooting, this driver will be loaded again in its default mode of operation, which is to reprogram any detected chips into vendor mode.

Executing:
```
rmmod cypress-serial
insmod cypress-serial
```
will reprogram chips to vendor mode.

Use path_only="usb path of serial dev" to reconfigure selected device only
```
rmmod cypress-serial
insmod cypress-serial cdc=y path_only="1-1.1"
```