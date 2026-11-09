#!/bin/sh

if [ $# -gt 0 ]; then
    a=$1
else
    a=1
fi

for i in $(seq 1 $a); do
    docker run -d --name worker_$i -v oslab_share:/data -e CONTAINER_ID=worker_$i concurrent-worker
done