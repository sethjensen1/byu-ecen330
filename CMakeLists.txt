cmake_minimum_required (VERSION 3.14.5)

project(ecen330)
include_directories(.)
include_directories(include)
include_directories(drivers)

# add_compile_options(-Wall)
# add_compile_options(-Wall -Wextra -pedantic -Werror)

#set(CMAKE_BUILD_TYPE Debug)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT EMU)
    # These are the options used to compile and run on the physical Zybo board    
    # You will need to compile using "cmake -DBOARD=1"
    
    # This sets up options for the ARM compiler
    include(platforms/zybo/xil_arm_toolchain/toolchain.cmake)

    # Places to search for .h header files
    include_directories(platforms/zybo/xil_arm_toolchain/bsp/ps7_cortexa9_0/include)

    # link_directories instructs the compiler where it should look for libraries.
    link_directories(platforms/zybo)
    link_directories(platforms/zybo/xil_arm_toolchain)

    # Only use this for lasertag.
    # link_directories(platforms/zybo/lasertag_libs)

    # Set this variable to the name of libraries that board executables need to link to
    set(330_LIBS c gcc zybo xil c)

    # Pass the BOARD variable to the compiler, so it can be used in #ifdef statements
    add_compile_definitions(ZYBO_BOARD=1)

else()
    # These options are required if you want to use the Zybo board emulator    

    # This sets up options for the compiler
    include (platforms/emulator/emu.cmake)

    # Places to search for .h header files
    include_directories(platforms/emulator/include)

    # link_directories instructs the compiler where it should look for libraries.
    link_directories(platforms/emulator)

    # Set this variable to the name of libraries that the emulator needs to link to
    set(330_LIBS emu Qt5Widgets Qt5Gui Qt5Core pthread)

    # Include this header file with all emulator builds
    add_definitions(-include emulator.h)
endif()

# Subdirectories to look for other CMakeLists.txt files
add_subdirectory(drivers)
add_subdirectory(lab1_helloworld)
add_subdirectory(lab2_gpio)
add_subdirectory(lab3_timer)
add_subdirectory(lab4_interrupts)
add_subdirectory(lab5_touchscreen)
add_subdirectory(lab6_clock)
add_subdirectory(lab7_tictactoe)
add_subdirectory(lab8_missilecommand)
add_subdirectory(lab9_project)

# The rest of this file is to add custom targets to the Makefile that is generated by CMake.

if (NOT EMU)
if (WSL) # Windows Subsystem for Linux
set(XIL_TOOL_PATH C:/Xilinx/Vivado/2020.2)
set(TEMP_PATH /mnt/c/temp/xilinx)

# make BOOT.bin ELF_PATH=...
# This command is used to create an SD card image.
add_custom_target(BOOT.bin
    COMMAND if [ -z \${ELF_PATH} ]\; then echo '\\033[0;31mSpecify an ELF_PATH. Example: make BOOT.bin ELF_PATH=labs/lab1.elf\\033[0m'\;exit 1\; fi
    COMMAND if [ ! -f \${ELF_PATH} ]\; then echo '\\033[0;31mThe file \${ELF_PATH} does not exist.\\033[0m'\;exit 1\; fi
    COMMAND mkdir -p ${TEMP_PATH}
    COMMAND cp ../platforms/hw/fsbl.elf ${TEMP_PATH}
    COMMAND cp ../platforms/hw/330_hw_system.bit ${TEMP_PATH}
    COMMAND cp \${ELF_PATH} ${TEMP_PATH}/program.elf
    COMMAND echo '//arch = zynq; split = false; format = BIN' > ${TEMP_PATH}/conf.bif
    COMMAND echo 'the_ROM_image:' >> ${TEMP_PATH}/conf.bif
    COMMAND echo '{' >> ${TEMP_PATH}/conf.bif
    COMMAND echo '  [bootloader]fsbl.elf' >> ${TEMP_PATH}/conf.bif
    COMMAND echo '  330_hw_system.bit' >> ${TEMP_PATH}/conf.bif
    COMMAND echo '  program.elf' >> ${TEMP_PATH}/conf.bif
    COMMAND echo '}' >> ${TEMP_PATH}/conf.bif
    COMMAND cmd.exe /C 'cd C:/temp/xilinx && ${XIL_TOOL_PATH}/bin/bootgen.bat -image conf.bif -arch zynq -o BOOT.bin -w on'
)

else() # Linux System
# set(XIL_TOOL_PATH /tools/Xilinx/Bootgen/2022.2)
set(XIL_TOOL_PATH /tools/Xilinx/Vivado/2023.1)

# make BOOT.bin ELF_PATH=...
# This command is used to create an SD card image.
add_custom_target(BOOT.bin
    COMMAND if [ -z \${ELF_PATH} ]\; then echo '\\033[0;31mSpecify an ELF_PATH. Example: make BOOT.bin ELF_PATH=labs/lab1.elf\\033[0m'\;exit 1\; fi
    COMMAND if [ ! -f \${ELF_PATH} ]\; then echo '\\033[0;31mThe file \${ELF_PATH} does not exist.\\033[0m'\;exit 1\; fi
    COMMAND echo '//arch = zynq; split = false; format = BIN' > conf.bif
    COMMAND echo 'the_ROM_image:' >> conf.bif
    COMMAND echo '{' >> conf.bif
    COMMAND echo '  [bootloader]../platforms/hw/fsbl.elf' >> conf.bif
    COMMAND echo '  ../platforms/hw/330_hw_system.bit' >> conf.bif
    COMMAND echo '  \${ELF_PATH}' >> conf.bif
    COMMAND echo '}' >> conf.bif
    COMMAND ${XIL_TOOL_PATH}/bin/bootgen -image conf.bif -arch zynq -o BOOT.bin -w on
)

endif() # WSL
endif() # NOT EMU
