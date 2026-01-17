#!/bin/bash
# build matrix:
# python: 3.10, 3.11, 3.12
# libboost: 1.84, 1.86


rm -rf output/ # clean previous builds
rm -rf rattl*.log
for platform in linux-64 osx-arm64; do
  for python_ver in 3.10 3.11 3.12; do
      for boost_ver in 1.84 1.86; do
        rattler-build build --recipe conda/meta.yaml --target-platform ${platform} --variant python=${python_ver} --variant libboost=${boost_ver} >& rattler-build-${platform}-${python_ver}-boost${boost_ver}.log
    done
  done
done