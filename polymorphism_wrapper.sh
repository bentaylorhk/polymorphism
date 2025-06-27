#!/bin/bash

# Benjamin Michael Taylor (bentaylorhk)
# 2025

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

BINARY_NAME="polymorphism"
BINARY_PATH="$SCRIPT_DIR/build/bin/$BINARY_NAME"

while true; do
    sleep 5
    "$BINARY_PATH"
    EXIT_CODE=$?
    echo "'$BINARY_NAME' exited with code $EXIT_CODE. Restarting in 5 seconds..."
done
