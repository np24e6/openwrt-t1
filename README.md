# Firmware Building And Installation

## Prerequisites

	Make sure you're running a supported Linux distribution.
	We recommend using Ubuntu 20.04 LTS (http://www.ubuntu.com/download/desktop).
	You might succeed with other distributions.

	It is recommended to use Docker environment to build RUTOS firmware which
	is accessible from "RUTX_R_GPL_00.07.07.2/scripts/dockerbuild".

## Building with Docker environment

	1. Install Docker (https://docs.docker.com/engine/install/ubuntu).

	2. Extract the  archive to an empty folder

		$ mkdir RUTX_R_GPL_00.07.07.2
		$ tar -xzf ~/Downloads/RUTX_R_GPL_00.07.07.2.tar.gz -C RUTX_R_GPL_00.07.07.2

	3. Update feeds

		$ cd RUTX_R_GPL_00.07.07.2
		$ ./scripts/dockerbuild ./scripts/feeds update -a

	4. Build the image

		$ ./scripts/dockerbuild make

	5. Prepare packages for offline installation (optional)

		$ ./scripts/dockerbuild make pm

		After this you can find packages in "RUTX_R_GPL_00.07.07.2/bin/packages/<arch_name>/zipped_packages".

	6. Sign the image with local keys (optional):

		This step is only necessary if the firmware needs to be flashed
		from the bootloader.

		$ ./scripts/dockerbuild make sign

	Note: do not launch the 'dockerbuild' script as root user (e.g. with sudo) - the script will use sudo itself, where appropriate.

	Note: Rootless docker is unsupported due to file ownership issues - files created on host will appear to be owned by 'root' user in the container.

## Building with Host OS

	1. Install NodeJS v18 (or newer) and NPM v9 (or newer).

		Please follow NodeJS and NPM installation guides for your distribution.

		Third-party helpers such as "nvm" (https://github.com/nvm-sh/nvm) or "n" (https://github.com/tj/n) can be used to manage multiple NodeJS and NPM versions on the system.

	2. Install required packages

		$ sudo apt update
		$ sudo apt install binutils binutils-gold bison build-essential bzip2 \
			ca-certificates curl default-jdk device-tree-compiler devscripts  \
			ecj file flex fuse g++ gawk gcc gcc-multilib gengetopt gettext    \
			git gnupg groff help2man java-wrappers java-propose-classpath jq  \
			libc6-dev libffi-dev libncurses5-dev libpcre3-dev libsqlite3-dev  \
			libssl-dev libxml-parser-perl lz4 make ocaml ocaml-findlib        \
			ocaml-nox patch pkg-config python3 python3-dev python3-distutils  \
			python3-yaml rsync ruby sharutils subversion swig u-boot-tools    \
			unzip uuid-dev vim-common wget zip zlib1g-dev


	3. Extract the  archive to an empty folder

		$ mkdir RUTX_R_GPL_00.07.07.2
		$ tar -xzf ~/Downloads/RUTX_R_GPL_00.07.07.2.tar.gz -C RUTX_R_GPL_00.07.07.2

	4. Update feeds

		$ cd RUTX_R_GPL_00.07.07.2
		$ ./scripts/feeds update -a

	5. Build the image

		$ make

	6. Prepare packages for offline installation (optional)

		$ make pm

		After this you can find packages in "RUTX_R_GPL_00.07.07.2/bin/packages/<arch_name>/zipped_packages".

	7. Sign the image with local keys (optional):

		This step is only necessary if the firmware needs to be flashed
		from the bootloader.

		$ make sign

## Installation

	After successful build you will get the firmware file in
		"RUTX_R_GPL_00.07.07.2/bin/targets/ipq40xx/generic/tltFws".

	Update the new firmware via the web interface on your device.

# Minimal image configuration

	You can set up a minimal image build where only core packages are included.
	Note: The device will be accessible only through Ethernet LAN and SSH.

	You can configure a minimal image by removing the .config file and running menuconfig:

		$ rm .config
		$ make menuconfig

	Then, navigate to "Target Images" and select "Build Minimal Image."

	After that, you can exit menuconfig, save the configuration, and build the image as stated in the previous steps.

	Note: if you're using dockerbuild, then run menuconfig through it as well:

		$ ./scripts/dockerbuild make menuconfig

# Firmware Rebranding

## WebUI rebranding

	All changes should be done in "RUTX_R_GPL_00.07.07.2/package/feeds/vuci/vuci-ui-core/bin/dist" folder.

	WebUI Colors can be changed in "brand/brand.css" file.
	Company information can be changed in "brand/brand.json" file.

	Logo and Icon changes:

		- Main WebUI logo is "tlt_networks_logo.svg".

		- Login page logo is "tlt-icons/tlt_networks_logo_white.svg".

		- Favicon is "favicon.ico" and "assets/favicon.ico".

		- All other icons can be found in "tlt-icons" folder.

## SSH Banner

	1. Install figlet tool:

		$ sudo apt install figlet

	2. Use figlet tool to generate needed text:

		$ figlet YOUR_TEXT > "RUTX_R_GPL_00.07.07.2/package/base-files/files/etc/banner.logo"

## Firmware Version Change

	Edit "gpl_version" file to change the prefix and/or version of the compiled firmware.

## Default IP Change

	1. Open configuration:

		$ make menuconfig
	
	2. Navigate to "Target Firmware options".

	3. Select "Default IP address".

	4. Enter new default IP address and exit the configuration.

## Default Password Change

	To change the default device password changes need to be made inside "RUTX_R_GPL_00.07.07.2/package/base-files/files/lib/preinit/84_set_password" file.
	Change "admin01" to your password on line [ -z "$passwd" ] && passwd="$(mkpasswd admin01)"
