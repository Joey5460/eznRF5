#!/bin/bash
if [ 'init' == "$1" ]; then
    ./dfu.sh mecc
    ./dfu.sh gkey
fi

./dfu.sh bootloader
./dfu.sh app
./dfu.sh setting
./dfu.sh hex
./dfu.sh dfu



