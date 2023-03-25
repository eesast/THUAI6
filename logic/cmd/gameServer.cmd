@echo off

<<<<<<< HEAD
start cmd /k ..\Server\bin\Debug\net6.0\Server.exe --port 8888 --teamCount 2 --playerNum 2 --gameTimeInSecond 600 --fileName test 
=======
start cmd /k ..\Server\bin\Debug\net6.0\Server.exe --port 8888 --teamCount 2 --playerCount 4 --playerNum 1 --gameTimeInSecond 600 --fileName test 
>>>>>>> 9497283ae5076d61510347911d4d9ac16841cef3

ping -n 2 127.0.0.1 > NUL

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --cl --port=8888 --characterID=0 --type=1 --occupation=1

ping -n 2 127.0.0.1 > NUL

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --cl --port=8888 --characterID=1 --type=1 --occupation=1
