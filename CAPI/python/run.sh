#!/usr/bin/env bash

python PyAPI/main.py -I 172.22.32.1 -P 8888 -p 0 -d -o &
python PyAPI/main.py -I 172.22.32.1 -P 8888 -p 1 -d &
python PyAPI/main.py -I 172.22.32.1 -P 8888 -p 2 -d &
python PyAPI/main.py -I 172.22.32.1 -P 8888 -p 3 -d &
# python PyAPI/main.py -I 172.22.32.1 -P 8888 -p 4 -d &