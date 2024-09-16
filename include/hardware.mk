#
# Copyright (C) 2023 Teltonika-Networks
#

#Bluetooth
HW_BT_LE:=Bluetooth low energy (LE) for short range communication

# Ethernet common
HW_ETH_SPEED_100:=10/100 Mbps
HW_ETH_SPEED_1000:=10/100/1000 Mbps
HW_ETH_RJ45_PORTS:=x RJ45 ports
HW_ETH_RJ45_PORT:=x RJ45 port
HW_ETH_WAN_PORT:=x WAN port
HW_ETH_WAN_PORTS:=x WAN ports
HW_ETH_LAN_PORT:=x LAN port
HW_ETH_LAN_PORTS:=x LAN ports
HW_ETH_ETH_PORT:=x ETH port
HW_ETH_ETH_PORTS:=x ETH ports
HW_ETH_SFP_PORT:=x SFP port
HW_ETH_SFP_PORTS:=x SFP ports

#physical interfaces
HW_INTERFACE_IO_4PIN_IOS:=2 x Digital Input, 2 x Digital Output on 4-pin power connector
HW_INTERFACE_IO_10PIN:=2 x Inputs and 2 x Outputs on 10-pin industrial socket, 1 x Digital input and 1 x Digital output on 4-pin power connector
HW_INTERFACE_IO_4PIN_IN_OUT:=1 x Digital Input, 1 x Digital Output on 4-pin power connector
HW_INTERFACE_IO_16PIN:=3 x Configurable digital I/O in 16-pin terminal block
HW_INTERFACE_RESET:=Reboot/User default reset/Factory reset button
HW_INTERFACE_SIM_HOLDER:=x SIM slot (Mini SIM - 2FF), 1.8 V/3 V, external SIM holder
HW_INTERFACE_SIM_HOLDERS:=x SIM slots (Mini SIM - 2FF), 1.8 V/3 V, external SIM holders
HW_INTERFACE_SIM_TRAY:=x SIM slots (Mini SIM - 2FF), 1.8 V/3 V, double stacked SIM tray
HW_INTERFACE_SIM_ESIM:=eSIM (Optional)
HW_INTERFACE_USB:=x USB A port for external devices
HW_INTERFACE_RS232_DB9:=x DB9 socket
HW_INTERFACE_RS232_4PIN:=4-pin in 16-pin terminal block (TX, RX, RTS, CTS)
HW_INTERFACE_RS232_6PIN:=x 6-pin industrial socket
HW_INTERFACE_RS485_4PIN:=4-pin in 16-pin terminal block (D+, D-, R+, R-)
HW_INTERFACE_RS485_6PIN:=x 6-pin industrial socket
HW_INTERFACE_POWER_2PIN:=1 x 2-pin power connector
HW_INTERFACE_POWER_4PIN:=1 x 4-pin power connector
HW_INTERFACE_POWER_16PIN:=1 x 16-pin terminal block
HW_INTERFACE_POWER_RJ45:=RJ45, PoE In, 42.5 - 57.0 VDC

# input/output
HW_INPUT_DI_30V:=x Digital Input, 0 - 6 V detected as logic low, 8 - 30 V detected as logic high
HW_INPUT_DI_50V:=x Digital Input, 0 - 6 V detected as logic low, 8 - 50 V detected as logic high
HW_OUTPUT_DO_30V:=x Digital Output, Open collector output, max output 30 V, 300 mA
HW_OUTPUT_DO_40V:=x Digital Output, Open collector output, max output 40 V, 400 mA
HW_OUTPUT_DO_50V:=x Digital Output, Open collector output, max output 50 V, 300 mA

# usb
HW_USB_2_DATA_RATE:=USB 2.0
HW_USB_APPLICATIONS:=Samba share, USB-to-serial
HW_USB_EXTERNAL_DEV:=Possibility to connect external HDD, flash drive, additional modem, printer, USB-serial adapter
HW_USB_STORAGE_FORMATS:=FAT, FAT32, exFAT, NTFS (read-only), ext2, ext3, ext4

# SD card
HW_SD_PHYSICAL_SIZE:=Micro SD
HW_SD_APLICATIONS:=Samba share, Storage Memory Expansion, DLNA
HW_SD_CAPACITY:=Up to 64 GB
HW_SD_STORAGE_FORMATS:=FAT32, NTFS, ext2, ext3, ext4

#serial
HW_SERIAL_RS232:=DB9 connector, RS232 (with RTS, CTS flow control)
HW_SERIAL_RS232_FLOW:=6-pin connector, RS232 (no flow control). 300 to 230400 baud rate
HW_SERIAL_RS485_HALF:=RS485 Half Duplex (2 wires). 300-2000000 baud rate
HW_SERIAL_RS485:=RS485 Full Duplex (4 wires) and Half Duplex (2 wires). 300-115200 baud rate

# physical specifications
HW_PHYSICAL_HOUSING_AL:=Aluminium housing
HW_PHYSICAL_HOUSING_AL_PL:=Aluminium housing, plastic panels
HW_PHYSICAL_HOUSING_AND_PANELS_AL:=Anodized aluminum housing and panels
HW_PHYSICAL_HOUSING_STEEL:=Full steel housing
HW_PHYSICAL_HOUSING_UV_PLASTIC:=UV stabilized plastic
HW_PHYSICAL_MOUNTING:=DIN rail, flat surface placement
HW_PHYSICAL_MOUNTING_KIT:=DIN rail, wall mount, flat surface (all require additional kit)
HW_PHYSICAL_MOUNTING_RACK:=Rack mounting kit

# operating environment
HW_OPERATING_TEMP:=-40 °C to 75 °C
HW_OPERATING_HUMIDITY:=10% to 90% non-condensing
HW_OPERATING_PROTECTION_IP30:=IP30
HW_OPERATING_PROTECTION_IP55:=IP55

# RAM parameters
HW_RAM_TYPE_DDR2:=DDR2
HW_RAM_TYPE_DDR3:=DDR3
HW_RAM_SIZE_64M:=64 MB
HW_RAM_SIZE_128M:=128 MB
HW_RAM_SIZE_256M:=256 MB
HW_RAM_SIZE_512M:=512 MB

# poe standards
HW_POE_STD_80203AF:=802.3af PoE Class 1
HW_POE_STD_80203AT:=802.03at

# flash parameters
HW_FLASH_SIZE_16M:=16 MB
HW_FLASH_SIZE_128M:=128 MB
HW_FLASH_SIZE_256M:=256 MB
HW_FLASH_SIZE_512M:=512 MB
HW_FLASH_TYPE_NOR:=NOR Flash
HW_FLASH_TYPE_SPI:=SPI Flash
HW_FLASH_TYPE_NOR_SERIAL:=serial NOR flash
HW_FLASH_TYPE_NAND_SERIAL:=serial NAND flash

# Wireless standards
HW_WIFI_4:=IEEE 802.11b/g/n (Wi-Fi 4)
HW_WIFI_5:=802.11b/g/n/ac Wave 2 (WiFi 5) with data transmission rates up to 867 Mbps (Dual Band, MU-MIMO), 802.11r fast transition, Access Point (AP), Station (STA)
HW_WIFI_50_USERS:=Up to 50 simultaneous connections
HW_WIFI_100_USERS:=Up to 100 simultaneous connections
HW_WIFI_150_USERS:=Up to 150 simultaneous connections

# power coonector
HW_POWER_CONNECTOR_RJ45:=RJ45 Socket
HW_POWER_CONNECTOR_16PIN:=2-pin in 16-pin industrial terminal block
HW_POWER_CONNECTOR_2PIN:=2-pin industrial DC power socket
HW_POWER_CONNECTOR_4PIN:=4-pin industrial DC power socket
HW_POWER_CONNECTOR_3PIN:=3-pos plugable terminal block

# power voltage range
HW_POWER_VOLTAGE_4PIN_30V:=9 - 30 VDC, reverse polarity protection, voltage surge/transient protection
HW_POWER_VOLTAGE_4PIN_50V:=9 - 50 VDC, reverse polarity protection, surge protection >51 VDC 10us max
HW_POWER_VOLTAGE_16PIN:=9 - 30 VDC, reverse polarity protection, voltage surge/transient protection
HW_POWER_VOLTAGE_RJ45:=42.5 - 57.0 VDC, reverse polarity protection, voltage surge/transient protection
HW_POWER_VOLTAGE_POE_2:=44.0 - 57.0 VDC

# PoE (passive) power options
HW_POWER_POE_PASSIVE_ACTIVE:=Passive and Active PoE. Possibility to power up through LAN1 port, compatible with IEEE802.3at.
HW_POWER_POE_PASSIVE_30V:=Passive PoE over spare pairs. Possibility to power up through LAN1 port, not compatible with IEEE802.3af, 802.3at and 802.3bt standards, Mode B, 9 - 30 VDC
HW_POWER_POE_PASSIVE_50V:=Possibility to power up through LAN1 port, not compatible with IEEE802.3af, 802.3at and 802.3bt standards, Mode B, 9 - 50 VDC
HW_POWER_POE_INSTALL:=Passive PoE can be installed upon request.

# Ethernet standards
HW_ETH_WAN_STANDARD:=compliance with IEEE 802.3, IEEE 802.3u, 802.3az standards, supports auto MDI/MDIX
HW_ETH_LAN_2_STANDARD:=compliance with IEEE 802.3, IEEE 802.3u, 802.3az standards, supports auto MDI/MDIX crossover
HW_ETH_LAN_STANDARD:=compliance with IEEE 802.3, IEEE 802.3u standards, supports auto MDI/MDIX

# emissions & imunity
HW_IMUNITY_EMISION_CS:=EN 61000-4-6:2014
HW_IMUNITY_EMISION_DIP:=EN 61000-4-11:2020
HW_IMUNITY_EMISION_EFT:=EN 61000-4-4:2012
HW_IMUNITY_EMISION_SURGE:=EN 61000-4-5:2014 + A1:2017
HW_IMUNITY_EMISION_ESD:=EN 61000-4-2:2009
HW_IMUNITY_EMISION_RI:=EN IEC 61000-4-3:2020

# emissions & immunity - standards
HW_EI_STANDARDS_EN_55032:=EN 55032:2015 + A11:2020
HW_EI_STANDARDS_EN_55035:=EN 55035:2017 + A11:2020
HW_EI_STANDARDS_EN_IEC_61000-3-2:=EN IEC 61000-3-2:2019 + A1:2021
HW_EI_STANDARDS_EN_61000-3-3:=EN 61000-3-3:2013 + A1:2019
HW_EI_STANDARDS_EN_301_489-1_V2.2.3:=EN 301 489-1 V2.2.3
HW_EI_STANDARDS_EN_301_489-17_V3.2.4:=EN 301 489-17 V3.2.4
HW_EI_STANDARDS_EN_301_489-52_V1.2.1:=EN 301 489-52 V1.2.1
HW_EI_STANDARDS_EN_IEC_61000-3-2:=EN IEC 61000-3-2:2019

#safety standards
HW_SAFETY_EN_IEC_62368-1:=EN IEC 62368-1:2020 + A11:2020
HW_SAFETY_EN_IEC_62311:=EN IEC 62311:2020
HW_SAFETY_EN_5066:=EN 50665:2017
HW_SAFETY_AS/NZS_62368:=AS/NZS 62368.1:2022
HW_SAFETY_IEC_62368-1:=EN IEC 62368-1:2018

# rf standards
HW_RF_EN_300_328_V2.2.2:=EN 300 328 V2.2.2
HW_RF_EN_301_511_V12.5.1:=EN 301 511 V12.5.1
HW_RF_EN_301_908-1_V15.2.1:=EN 301 908-1 V15.2.1
HW_RF_EN_301_908-2_V13.1.1:=EN 301 908-2 V13.1.1
HW_RF_EN_301_908-13-V13.1.1:=EN 301 908-13 V13.1.1
HW_RF_EN_301_908-1_V15.1.1:=EN 301 908-1 V15.1.1
HW_RF_EN_301_908-13_V13.2.1:=EN 301 908-13 V13.2.1
