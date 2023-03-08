#! /bin/sh

set -e
script_dir=$(dirname "$0")

brew install ninja
cd "${script_dir}"/..
CMAKE_GENERATOR=Ninja ./update_external_sources.sh


