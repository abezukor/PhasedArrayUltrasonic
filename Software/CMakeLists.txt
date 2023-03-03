# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(threeDPhasedArray C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Add compilt commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile")

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(threeDPhasedArray)

pico_generate_pio_header(threeDPhasedArray ${CMAKE_CURRENT_LIST_DIR}/pulses_pio/pulses.pio)

target_sources(threeDPhasedArray PRIVATE 
    threeDPhasedArray.cpp 
    pulses_pio/pulses_pio.cpp
    PhasedArrayPulses.cpp 
    pio_irh.cpp
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_pio
    hardware_clocks
    pico_multicore
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)