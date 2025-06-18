#!/bin/bash
export PICO_SDK_PATH="$HOME/.pico-sdk"
export FREERTOS_KERNEL_PATH="$HOME/.FreeRTOS-Kernel"

rm -rf build
mkdir build
cd build
cmake ..
cd ..

