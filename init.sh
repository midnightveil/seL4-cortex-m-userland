#!/usr/bin/env sh

set -euo pipefail

# Determine path to this script (fast, cheap "dirname").
SCRIPT_PATH=${0%/*}
# Save script name for diagnostic messages (fast, cheap "basename").
SCRIPT_NAME=${0##*/}

# Ensure script path and current working directory are not the same.
if [ "$PWD" = "$SCRIPT_PATH" ]
then
    echo "\"$SCRIPT_NAME\" should not be invoked from top-level directory" >&2
    exit 1
fi

# Try and make sure we weren't invoked from a source directory by checking for a
# CMakeLists.txt file.
if [ -e CMakeLists.txt ]
then
    echo "\"$SCRIPT_NAME\" should be invoked from a build directory and not" \
        "source directories containing a CMakeLists.txt file" >&2
    exit 1
fi

cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DLLVM_TOOLCHAIN=ON \
    -DKernelVerificationBuild=OFF \
    "$SCRIPT_PATH/seL4" "$@"
