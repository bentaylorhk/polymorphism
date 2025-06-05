#!/bin/bash
# Benjamin Michael Taylor (bentaylorhk)
# 2025

BINARY_PATH="/home/ben/repos/polyphonic-rsvp/build/src/polyphonic_rsvp"

while true; do
    sleep 5
    "$BINARY_PATH"
    EXIT_CODE=$?
    echo "polyphonic-rsvp exited with code $EXIT_CODE. Restarting in 5 seconds..."
done
