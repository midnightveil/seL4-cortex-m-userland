#!/usr/bin/env sh

set -euo pipefail

# M33
qemu-system-arm -M mps2-an521 -nographic -kernel "$@"
