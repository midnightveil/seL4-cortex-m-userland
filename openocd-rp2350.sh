#!/usr/bin/env sh

set -euo pipefail

# rpi foundation fork
openocd -f interface/cmsis-dap.cfg -f target/rp2350.cfg -c "adapter speed 5000"
