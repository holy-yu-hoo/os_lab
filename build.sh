#!/bin/sh

set -e

mkdir -p build

g++ main.cpp -o ./build/main -O2 -static
echo "compiled"

docker volume create oslab_share
echo "volume created"


docker build -t concurrent-worker .
echo "image created"