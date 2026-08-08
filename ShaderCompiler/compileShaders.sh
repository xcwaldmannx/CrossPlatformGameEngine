#!/usr/bin/env bash

set -euo pipefail

SHADER_ROOT="CrossPlatformGameEngine/src/shaders"

find "$SHADER_ROOT" -type f \( \
    -name "*.vert" -o \
    -name "*.frag" -o \
    -name "*.comp" \
\) -print0 |
while IFS= read -r -d '' shader; do
    output="${shader%.*}.spv"

    echo "Compiling: $shader"
    echo "       -> $output"

    glslc "$shader" -o "$output"
done

echo "All shaders compiled successfully."