#!/usr/bin/bash
# Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
set -eou pipefail

for dep in "$@"; do
    (
        cd "vendor/$dep" && \
            mkdir -p build && cd build \
            && cmake .. && \
            make -j4 && make install
    )
done
