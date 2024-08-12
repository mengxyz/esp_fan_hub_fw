#!/bin/bash

# curl -F "file=@./.pio/build/esp-fan-hub/firmware.bin" http://192.168.1.183/update
curl -u user:123456 -X POST -# --max-time 10 -F "file=@./.pio/build/esp-fan-hub/firmware.bin" http://192.168.1.183/update
