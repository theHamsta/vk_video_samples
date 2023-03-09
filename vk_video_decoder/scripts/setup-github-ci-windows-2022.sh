#! /bin/sh

set -e

script_dir=$(dirname $0)
cd ${script_dir}/..
chmod +x ./update_external_sources.bat
./update_external_sources.bat

cd ..
curl -L https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-lgpl-shared.zip --output ffmpeg.zip
unzip ffmpeg.zip
mv ffmpeg*-shared ffmpeg
