rm -rf lib
mkdir lib
cd lib
git clone https://github.com/RaspberryPi/pico-sdk --recurse-submodules
git clone -b smp https://github.com/FreeRTOS/FreeRTOS-Kernel --recurse-submodules

export PICO_SDK_PATH=$PWD/pico-sdk
export FREERTOS_KERNEL_PATH=$PWD/FreeRTOS-Kernel
