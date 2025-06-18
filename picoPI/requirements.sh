#!/bin/bash
set -e

echo "Installing system dependencies for Raspberry Pi Pico development..."

# Update package lists
sudo apt update

# Install build tools
sudo apt install -y \
  cmake \
  build-essential \
  gcc-arm-none-eabi \
  gdb-multiarch \
  libnewlib-arm-none-eabi

echo "✅ Dependencies installed."
