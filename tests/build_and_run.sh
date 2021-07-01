#!/bin/bash

set -e

make fluid_wm

XEPHYR=$(whereis -b Xephyr | cut -f2 -d' ')
xinit ./xinitrc -- \
    "$XEPHYR" \
        :100 \
        -ac \
        -screen 1600x900 \
        -host-cursor
        -host-grab-key
