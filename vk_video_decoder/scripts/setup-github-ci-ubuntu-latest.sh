#! /bin/sh

set -e

script_dir=$(dirname "$0")
sudo  apt-get install -y \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    ninja-build \
    libxcb-keysyms1-dev \
    libxcb-dri3-dev \
    libx11-dev \
    libwayland-dev \
    libxrandr-dev \
    libxcb-ewmh-dev \
    libvulkan1 \
    libvulkan-dev
cd "${script_dir}"/..
CMAKE_GENERATOR=Ninja ./update_external_sources.sh
