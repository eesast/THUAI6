@echo off

start cmd /k ..\Server\bin\Debug\net6.0\Server.exe --ip  0.0.0.0  --port 8888 --studentCount 4 --trickerCount 1 --gameTimeInSecond 600 --fileName video --mapResource ".\map\map2_final.txt"

ping -n 2 127.0.0.1 > NUL

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --port 8888 --characterID 4 --type 2 --occupation 1

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --port 8888 --characterID 0 --type 1 --occupation 0

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --port 8888 --characterID 1 --type 1 --occupation 0

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --port 8888 --characterID 2 --type 1 --occupation 0

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --port 8888 --characterID 3 --type 1 --occupation 0

::start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --port 8888 --characterID 2030