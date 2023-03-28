@echo off

start cmd /k ..\Server\bin\Debug\net6.0\Server.exe --ip 0.0.0.0  --port 8888 --studentCount 2 --trickerCount 1 --gameTimeInSecond 600 --fileName test 

ping -n 2 127.0.0.1 > NUL

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --cl --port 8888 --characterID 4 --type 2 --occupation 1

ping -n 2 127.0.0.1 > NUL
