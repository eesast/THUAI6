@echo off

start cmd /k ..\Server\bin\Debug\net6.0\Server.exe --port 8888 --teamCount 2 --playerCount 2 --gameTimeInSecond 600 --fileName test 

ping -n 4 127.0.0.1 > NUL

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --cl --port=8888 --characterID=4 --type=2 --occupation=1

