#!/usr/bin/env sh

set -euo pipefail

qemu-system-arm -M mps2-an386 -nographic -kernel kernel.elf
