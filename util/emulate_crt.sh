#!/bin/bash

# Benjamin Michael Taylor (bentaylorhk)
# 2025

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BIN_DIR="$( dirname "$SCRIPT_DIR" )/build/bin"

alacritty --class crt_test -e sh -c "cd $BIN_DIR; exec bash" &
