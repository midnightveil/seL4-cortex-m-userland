#!/usr/bin/env sh

set -euo pipefail

# use --list-params
~/ARM/avh-linux-x86/bin/FVP_MPS2_Cortex-M33 \
   -C cpu0.semihosting-enable=0 \
   -C fvp_mps2.UART0.out_file='-' \
   -q \
   -a "$@"
