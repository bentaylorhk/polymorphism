#!/bin/bash

# Benjamin Michael Taylor (bentaylorhk)
# 2025

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PARENT_DIR="$( dirname "$SCRIPT_DIR" )"

WIDTH=800
HEIGHT=600

ROWS=19
COLS=55

alacritty --class crt_test
