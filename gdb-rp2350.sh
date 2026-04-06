#!/usr/bin/env bash

set -eo pipefail

gdb -ex 'set confirm off' -ex 'add-symbol-file userland/roottask/roottask.elf' -ex 'add-symbol-file kernel/kernel.elf' -ex 'tar ext :3333' -ex 'monitor reset init' -ex 'load meL4.elf' -ex continue
