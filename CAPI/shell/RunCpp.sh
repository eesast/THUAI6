#!/usr/bin/env bash

./CAPI/cpp/build/capi -I 127.0.0.1 -P 8888 -p 0 -d -o &
./CAPI/cpp/build/capi -I 127.0.0.1 -P 8888 -p 1 -d -o &
./CAPI/cpp/build/capi -I 127.0.0.1 -P 8888 -p 2 -d -o &
./CAPI/cpp/build/capi -I 127.0.0.1 -P 8888 -p 3 -d -o &
./CAPI/cpp/build/capi -I 127.0.0.1 -P 8888 -p 4 -d -o &