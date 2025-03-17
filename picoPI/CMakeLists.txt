cmake_minimum_required(VERSION 3.13)

# Pull in SDK
include(lib/pico-sdk/external/pico_sdk_import.cmake)

# Pull in FreeRTOS
include(lib/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/pico_sdk_import.cmake)
include(lib/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(app C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(WIFI_SSID "Kasper-2.4GHz")
set(WIFI_PASSWORD "3cc62d647")
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Initialize the SDK
pico_sdk_init()

add_compile_definitions(PICO_CYW43_ARCH_THREADSAFE=1)


add_executable(app src/main.c)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}
    src/inc
    lib/pico-sdk/lib/cyw43-driver/src
    )
 
# pull in common dependencies
target_link_libraries(app
    pico_stdlib
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    tinyusb_device
    tinyusb_board
    pico_cyw43_arch_lwip_threadsafe
    pico_lwip_freertos
    pico_btstack_cyw43
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_iperf
    cyw43-driver
  )

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(app)

# enable usb output, disable uart output
pico_enable_stdio_usb(app 1)
pico_enable_stdio_uart(app 0)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/inc)

target_compile_definitions(app PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    )