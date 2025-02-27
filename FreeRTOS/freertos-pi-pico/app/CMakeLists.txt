cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(/home/kasper/Documents/Civilingenioer_Robotteknologi/ISA_IPC/FreeRTOS/freertos-pi-pico/app/lib/pico-sdk/external/pico_sdk_import.cmake)

project(app C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add FreeRTOS Kernel as a subdirectory (Make sure it's correctly referenced)
add_subdirectory(/home/kasper/Documents/Civilingenioer_Robotteknologi/ISA_IPC/FreeRTOS/freertos-pi-pico/app/lib/FreeRTOS-Kernel)

# Define the executable
add_executable(app src/main.c)

# Include necessary directories
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    include
    lib/pico-sdk/external
    lib/FreeRTOS-Kernel/include
    lib/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include
)

# Link with FreeRTOS and Pico SDK
target_link_libraries(app
    pico_stdlib
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)

# Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(app)

# Enable USB and UART output
pico_enable_stdio_usb(app 1)
pico_enable_stdio_uart(app 1)
