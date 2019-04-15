#!/usr/bin/bash

set -eou pipefail

for dep in "$@"; do
    (
        cd "vendor/$dep" && \
            mkdir -p build && cd build \
            && cmake .. && \
            make -j4 && make install
    )
done
