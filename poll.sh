#!/bin/bash

SERIAL_PORT="/dev/cu.usbserial-14720"  # Replace with your serial port (e.g., COM3 on Windows)
BAUD_RATE=9600              # Adjust to match your device's baud rate

for SLAVE_ID in {1..10}; do
    echo "Scanning Modbus slave at address $SLAVE_ID..."
    mbpoll -m rtu -a $SLAVE_ID -r 1 -c 1 -t 3 -b $BAUD_RATE -P none $SERIAL_PORT 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "Slave found at address: $SLAVE_ID"
    fi
done