#! /bin/sh

set -e

script_dir=$(dirname "$0")
sudo apt-get install -y libavcodec-dev libavformat-dev libavutil-dev ninja-build
cd "${script_dir}"/..
CMAKE_GENERATOR=Ninja ./update_external_sources.sh
