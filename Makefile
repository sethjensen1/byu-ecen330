
ROOT_DIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
TOOLS_DIR = $(ROOT_DIR)/tools

EXECUTABLES_REQUIRED = make qtconfig clang-format-12 zip cmake 
EXECUTABLES_OPTIONAL = arm-none-eabi-gcc openocd

GXX_BAREMETAL_ARM_2020-2 = $(TOOLS_DIR)/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-g++

TEMPD= $(shell mktemp -d)


require:
	@echo "Checking that required programs are installed...";
	@for exe in $(EXECUTABLES_REQUIRED); do \
		command -v $$exe >/dev/null || (echo "ERROR: $$exe is not installed."; exit 1) || exit 1; \
	done
	@for exe in $(EXECUTABLES_OPTIONAL); do \
		command -v $$exe >/dev/null || (echo "WARNING: $$exe is not installed. This is required if you want to run on a physical board.");\
	done
	@echo "Done."

setup: require $(GXX_BAREMETAL_ARM_2020-2)
	git submodule init
	git submodule update

install:
# Update package repositories
	sudo apt update

# Install these packages using apt
	sudo apt install -y \
		make \
		python3-pip \
		build-essential \
		qtbase5-dev \
		clang-format \
		zip \
		python3-pip \
		x11-apps \
		cmake

install_board:
	sudo apt install -y \
		gcc-arm-none-eabi \
		python3-serial \
		xterm \
		openocd

$(GXX_BAREMETAL_ARM_2020-2): $(TOOLS_DIR)
	cd $(TEMPD) && \
	rm -rf gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2* && \
	wget "https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2?revision=108bd959-44bd-4619-9c19-26187abf5225&hash=46AF221F493505D04113CD7FD10F378688940FC8" && \
	tar -xf "gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2?revision=108bd959-44bd-4619-9c19-26187abf5225&hash=46AF221F493505D04113CD7FD10F378688940FC8" && \
	cp -r gcc-arm-none-eabi-9-2019-q4-major $(TOOLS_DIR)/
	rm -rf $(TEMPD)

