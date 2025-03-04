cd lib
if [ -z "$(ls -A pico-sdk)" ]; then
    git clone https://github.com/RaspberryPi/pico-sdk --recurse-submodules
fi

if [ -z "$(ls -A FreeRTOS-Kernel)" ]; then
    git clone -b smp https://github.com/FreeRTOS/FreeRTOS-Kernel --recurse-submodules
fi
export PICO_SDK_PATH=$PWD/pico-sdk
export FREERTOS_KERNEL_PATH=$PWD/FreeRTOS-Kernel
cd ..
