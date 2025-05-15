export PICO_SDK_PATH=~/.pico-sdk/
export FREERTOS_KERNEL_PATH=~/.FreeRTOS-Kernel/

rm -rf build
mkdir build
cd build
cmake ..
cd ..