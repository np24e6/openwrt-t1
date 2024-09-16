include $(INCLUDE_DIR)/hardware.mk

define Device/teltonika_rutx_common
	HARDWARE/Wireless/Wireless_mode := $(HW_WIFI_5)
	HARDWARE/Wireless/WIFI_users := $(HW_WIFI_150_USERS)
	HARDWARE/WAN/Port := 1 $(HW_ETH_WAN_PORT)
	HARDWARE/WAN/Speed := $(HW_ETH_SPEED_1000)
	HARDWARE/WAN/Standard := $(HW_ETH_LAN_2_STANDARD)
	HARDWARE/LAN/Port := 3 $(HW_ETH_LAN_PORTS)
	HARDWARE/LAN/Speed := $(HW_ETH_SPEED_1000)
	HARDWARE/LAN/Standard := $(HW_ETH_LAN_2_STANDARD)
	HARDWARE/System_Characteristics/CPU := Quad-core ARM Cortex A7, 717 MHz
	HARDWARE/System_Characteristics/RAM := $(HW_RAM_SIZE_256M), $(HW_RAM_TYPE_DDR3)
	HARDWARE/System_Characteristics/Flash_Storage := $(HW_FLASH_SIZE_256M), $(HW_FLASH_TYPE_SPI)
	HARDWARE/USB/Data_rate := $(HW_USB_2_DATA_RATE)
	HARDWARE/USB/Applications := $(HW_USB_APPLICATIONS)
	HARDWARE/USB/External_devices := $(HW_USB_EXTERNAL_DEV)
	HARDWARE/USB/Storage_formats := $(HW_USB_STORAGE_FORMATS)
	HARDWARE/Input_Output/Input := 1 $(HW_INPUT_DI_30V)
	HARDWARE/Input_Output/Output := 1 $(HW_OUTPUT_DO_30V)
	HARDWARE/Power/Connector := $(HW_POWER_CONNECTOR_4PIN)
	HARDWARE/Power/Input_voltage_range := 9 - 50 VDC, reverse polarity protection, voltage surge/transient protection
	HARDWARE/Power/PoE_Standards := $(HW_POWER_POE_PASSIVE_50V)
	HARDWARE/Physical_Interfaces/Ethernet := 4 $(HW_ETH_RJ45_PORTS), $(HW_ETH_SPEED_1000)
	HARDWARE/Physical_Interfaces/IO := $(HW_INTERFACE_IO_4PIN_IN_OUT)
	HARDWARE/Physical_Interfaces/Power := $(HW_INTERFACE_POWER_4PIN)
	HARDWARE/Physical_Interfaces/USB := 1 $(HW_INTERFACE_USB)
	HARDWARE/Physical_Interfaces/Reset := $(HW_INTERFACE_RESET)
	HARDWARE/Physical_Specification/Casing_material := $(HW_PHYSICAL_HOUSING_AL)
	HARDWARE/Physical_Specification/Mounting_options := $(HW_PHYSICAL_MOUNTING_KIT)
	HARDWARE/Operating_Environment/Operating_Temperature := $(HW_OPERATING_TEMP)
	HARDWARE/Operating_Environment/Operating_Humidity := $(HW_OPERATING_HUMIDITY)
	HARDWARE/Operating_Environment/Ingress_Protection_Rating := $(HW_OPERATING_PROTECTION_IP30)
endef

define Device/TEMPLATE_teltonika_rutx08
	$(Device/teltonika_rutx_common)
	DEVICE_MODEL := RUTX08

	HARDWARE/Wireless/Wireless_mode :=
	HARDWARE/Wireless/WIFI_users :=
	HARDWARE/Power/Power_consumption := 6 W Max
	HARDWARE/Physical_Interfaces/Status_leds := 8 x LAN status LEDs, 1 x Power LED
	HARDWARE/Physical_Specification/Weight := 345 g
	HARDWARE/Physical_Specification/Dimensions := 115 x 32.2 x 95.2 mm
endef
TARGET_DEVICES += TEMPLATE_teltonika_rutx08

define Device/TEMPLATE_teltonika_rutx09
	$(Device/teltonika_rutx_common)
	DEVICE_MODEL := RUTX09

	HARDWARE/Wireless/Wireless_mode :=
	HARDWARE/Wireless/WIFI_users :=
	HARDWARE/Power/Power_consumption := 9 W Max
	HARDWARE/Physical_Interfaces/Status_leds := 3 x WAN type, 2 x Mobile connection type, 5 x Mobile connection strength, 8 x LAN status, 3 x WAN status, 1x Power
	HARDWARE/Physical_Interfaces/SIM := 2 $(HW_INTERFACE_SIM_HOLDERS)
	HARDWARE/Physical_Interfaces/Antennas := 2 x SMA for LTE, 1 x SMA for GNNS
	HARDWARE/Physical_Specification/Weight := 455 g
	HARDWARE/Physical_Specification/Dimensions := 115 x 44.2 x 95.1 mm
endef
TARGET_DEVICES += TEMPLATE_teltonika_rutx09

define Device/TEMPLATE_teltonika_rutx10
	$(Device/teltonika_rutx_common)
	DEVICE_MODEL := RUTX10

	HARDWARE/Bluetooth/Bluetooth_4.0 := $(HW_BT_LE)
	HARDWARE/Power/Power_consumption := 9 W Max
	HARDWARE/Physical_Interfaces/Status_leds := 8 x LAN status LEDs, 1 x Power LED, 2 x 2.4G and 5G Wi-Fi LEDs
	HARDWARE/Physical_Interfaces/Antennas := 2 x RP-SMA for Wi-Fi, 1 x RP-SMA for Bluetooth
	HARDWARE/Physical_Specification/Weight := 355 g
	HARDWARE/Physical_Specification/Dimensions := 115 x 32.2 x 95.2 mm
endef
TARGET_DEVICES += TEMPLATE_teltonika_rutx10

define Device/TEMPLATE_teltonika_rutx11
	$(Device/teltonika_rutx_common)
	DEVICE_MODEL := RUTX11

	HARDWARE/Bluetooth/Bluetooth_4.0 := $(HW_BT_LE)
	HARDWARE/Power/Input_voltage_range := 9 - 50 VDC, reverse polarity protection, voltage surge/transient protection; 24 - 36 VDC for railway version of the code RUTX11 020G00
	HARDWARE/Power/Power_consumption := 16 W Max
	HARDWARE/Physical_Interfaces/Status_leds := 4 x WAN type LEDs, 2 x Mobile connection type, 5 x Mobile connection strength, 8 x LAN status, 1 x Power, 2 x 2.4G and 5G Wi-Fi
	HARDWARE/Physical_Interfaces/SIM := 2 $(HW_INTERFACE_SIM_HOLDERS)
	HARDWARE/Physical_Interfaces/Antennas := 2 x SMA for Mobile, 2 x RP-SMA for Wi-Fi, 1 x RP-SMA for Bluetooth, 1 x SMA for GNSS
	HARDWARE/Physical_Specification/Weight := 456 g
	HARDWARE/Physical_Specification/Dimensions := 115 x 44.2 x 95.1 mm
endef
TARGET_DEVICES += TEMPLATE_teltonika_rutx11

define Device/TEMPLATE_teltonika_rutx12
	$(Device/teltonika_rutx_common)
	DEVICE_MODEL := RUTX12

	HARDWARE/Bluetooth/Bluetooth_4.0 := $(HW_BT_LE)
	HARDWARE/LAN/Port := 4 $(HW_ETH_LAN_PORTS)
	HARDWARE/Power/Power_consumption := Idle < 4 W, Max < 22 W
	HARDWARE/Physical_Interfaces/Ethernet := 5 $(HW_ETH_RJ45_PORTS), $(HW_ETH_SPEED_1000)
	HARDWARE/Physical_Interfaces/Status_leds := 4 x connection status LEDs, 6 x connection strength LEDs, 10 x Ethernet port status LEDs, 4 x WAN status LEDs, 1 x Power LED, 2 x 2.4G and 5G Wi-Fi LEDs
	HARDWARE/Physical_Interfaces/SIM := 2 $(HW_INTERFACE_SIM_HOLDERS)
	HARDWARE/Physical_Interfaces/Antennas := 4 x SMA for LTE, 2 x RP-SMA for WiFi, 1 x RP-SMA for Bluetooth, 1 x SMA for GNNS
	HARDWARE/Physical_Specification/Weight := 540 g
	HARDWARE/Physical_Specification/Dimensions := 132 x 44.2 x 95.1 mm
endef
TARGET_DEVICES += TEMPLATE_teltonika_rutx12

define Device/TEMPLATE_teltonika_rutx14
	$(Device/teltonika_rutx_common)
	DEVICE_MODEL := RUTX14

	HARDWARE/Bluetooth/Bluetooth_4.0 := $(HW_BT_LE)
	HARDWARE/LAN/Port := 4 $(HW_ETH_LAN_PORTS)
	HARDWARE/Power/Power_consumption := Idle < 4 W, Max < 22 W
	HARDWARE/Physical_Interfaces/Ethernet := 5 $(HW_ETH_RJ45_PORTS), $(HW_ETH_SPEED_1000)
	HARDWARE/Physical_Interfaces/Status_leds := 2 x connection status LEDs, 3 x connection strength LEDs, 10 x Ethernet port status LEDs, 4 x WAN status LEDs, 1 x Power LED, 2 x 2.4G and 5G Wi-Fi LEDs
	HARDWARE/Physical_Interfaces/SIM := 2 $(HW_INTERFACE_SIM_HOLDERS)
	HARDWARE/Physical_Interfaces/Antennas := 4 x SMA for LTE, 2 x RP-SMA for WiFi, 1 x RP-SMA for Bluetooth, 1 x SMA for GNNS
	HARDWARE/Physical_Specification/Weight := 515 g
	HARDWARE/Physical_Specification/Dimensions := 132 x 44 x 95 mm
endef
TARGET_DEVICES += TEMPLATE_teltonika_rutx14

define Device/TEMPLATE_teltonika_rutx50
	$(Device/teltonika_rutx_common)
	DEVICE_MODEL := RUTX50

	HARDWARE/System_Characteristics/RAM := $(HW_RAM_SIZE_256M) (100 MB available for userspace)
	HARDWARE/System_Characteristics/Flash_Storage := $(HW_FLASH_SIZE_256M) (80 MB available for userspace)
	HARDWARE/LAN/Port := 4 $(HW_ETH_LAN_PORTS)
	HARDWARE/Power/Power_consumption := Idle < 4 W, Max < 18 W
	HARDWARE/Power/Input_voltage_range := $(HW_POWER_VOLTAGE_4PIN_50V)
	HARDWARE/Physical_Interfaces/Ethernet := 5 $(HW_ETH_RJ45_PORTS), $(HW_ETH_SPEED_1000)
	HARDWARE/Physical_Interfaces/Status_leds := 3 x connection status LEDs, 3 x connection strength LEDs, 10 x Ethernet port status LEDs, 4 x WAN status LEDs, 1 x Power LED, 2 x 2.4G and 5G Wi-Fi LEDs
	HARDWARE/Physical_Interfaces/SIM := 2 $(HW_INTERFACE_SIM_HOLDERS)
	HARDWARE/Physical_Interfaces/Antennas := 4 x SMA for Mobile, 2 x RP-SMA for Wi-Fi, 1 x SMA for GNNS
	HARDWARE/Physical_Specification/Weight := 533 g
	HARDWARE/Physical_Specification/Dimensions := 132 x 44.2 x 95.1 mm
endef
TARGET_DEVICES += TEMPLATE_teltonika_rutx50

define Device/TEMPLATE_teltonika_rutxr1
	$(Device/teltonika_rutx_common)
	DEVICE_MODEL := RUTXR1

	HARDWARE/LAN/Port := 4 $(HW_ETH_LAN_PORTS)
	HARDWARE/Fibre/Port := 1 $(HW_ETH_SFP_PORT) (cannot work simultaneously with Ethernet WAN port)
	HARDWARE/Input_Output/Input :=
	HARDWARE/Input_Output/Output :=
	HARDWARE/Power/Connector := 2 x 4-pin industrial DC power sockets for main and redundancy power source
	HARDWARE/Power/Power_consumption := Idle < 3 W, Max < 18 W
	HARDWARE/Power/PoE_Standards :=
	HARDWARE/Physical_Interfaces/IO :=
	HARDWARE/Physical_Interfaces/Ethernet := 5 $(HW_ETH_RJ45_PORTS), $(HW_ETH_SPEED_1000)
	HARDWARE/Physical_Interfaces/Status_leds := 2 x WAN type, 2 x Mobile connection type, 3 x Mobile signal strength, 2 x active SIM, 10 x Ethernet status, 2 x Console status, 2 x Power
	HARDWARE/Physical_Interfaces/SIM := 2 $(HW_INTERFACE_SIM_HOLDERS)
	HARDWARE/Physical_Interfaces/Antennas := 2 x SMA for LTE, 2 x RP-SMA for Wi-Fi
	HARDWARE/Physical_Interfaces/Power := 2 x 4-pin DC connector
	HARDWARE/Physical_Interfaces/Fibre := 1 $(HW_ETH_SFP_PORT)
	HARDWARE/Physical_Specification/Weight := 1093 g
	HARDWARE/Physical_Specification/Dimensions := 272 x 42.6 x 122.6 mm
	HARDWARE/Physical_Specification/Casing_material := $(HW_PHYSICAL_HOUSING_STEEL)
	HARDWARE/Physical_Specification/Mounting_options := $(HW_PHYSICAL_MOUNTING_RACK)
	HARDWARE/Operating_Environment/Ingress_Protection_Rating :=
endef
TARGET_DEVICES += TEMPLATE_teltonika_rutxr1
