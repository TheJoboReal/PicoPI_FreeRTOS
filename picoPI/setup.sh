#!/bin/bash
set -e  # Exit immediately if a command exits with a non-zero status
set -x  # Print each command before executing it (for debugging)


# Clone the repositories
if [ ! -d "$HOME/.pico-sdk" ]; then
    git clone --recurse-submodules https://github.com/RaspberryPi/pico-sdk "$HOME/.pico-sdk"
else
    echo "$HOME/.pico-sdk already exists, skipping clone."
fi

if [ ! -d "$HOME/.FreeRTOS-Kernel" ]; then
    git clone --recurse-submodules -b smp https://github.com/FreeRTOS/FreeRTOS-Kernel "$HOME/.FreeRTOS-Kernel"
else
    echo "$HOME/.FreeRTOS-Kernel already exists, skipping clone."
fi
