#! /bin/sh

script_dir=$(dirname "$0")
apt-get install -y libavcodec-dev libavformat-dev libavutil-dev ninja-build
cd "${script_dir}"/..
./update_external_sources.sh
