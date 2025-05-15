#!/bin/bash
set -e  # Exit immediately if a command exits with a non-zero status
set -x  # Print each command before executing it (for debugging)


# Clone the repositories
git clone --recurse-submodules https://github.com/RaspberryPi/pico-sdk ~/.pico-sdk
git clone --recurse-submodules -b smp https://github.com/FreeRTOS/FreeRTOS-Kernel ~/.FreeRTOS-Kernel

# Export environment variables
export PICO_SDK_PATH="$HOME/.pico-sdk"
export FREERTOS_KERNEL_PATH="$HOME/.FreeRTOS-Kernel"

