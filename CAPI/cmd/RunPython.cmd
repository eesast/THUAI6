@echo off

start cmd /k python .\CAPI\python\PyAPI\main.py -I 127.0.0.1 -P 8888 -p 0 -d -o 
start cmd /k python .\CAPI\python\PyAPI\main.py -I 127.0.0.1 -P 8888 -p 1 -d -o
start cmd /k python .\CAPI\python\PyAPI\main.py -I 127.0.0.1 -P 8888 -p 2 -d -o
start cmd /k python .\CAPI\python\PyAPI\main.py -I 127.0.0.1 -P 8888 -p 3 -d -o
start cmd /k python .\CAPI\python\PyAPI\main.py -I 127.0.0.1 -P 8888 -p 4 -d -o
