#!/bin/bash -x

set -o errexit
set -o nounset
set -o pipefail

mkdir -p build
(
  cd build
  cmake -G"Unix Makefiles" -fopt-info-vec-missed -fopt-info-vec ..
  make
)
rm -rf build
