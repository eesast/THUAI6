@echo off

<<<<<<< HEAD
start cmd /k ..\Server\bin\Debug\net6.0\Server.exe --port 8888 --studentCount 1 --trickerCount 1 --gameTimeInSecond 600 --fileName test 

ping -n 2 127.0.0.1 > NUL

::start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --cl --port=8888 --characterID=4 --type=2 --occupation=1
=======
start cmd /k ..\Server\bin\Debug\net6.0\Server.exe --ip 0.0.0.0  --port 8888 --studentCount 1 --trickerCount 1 --gameTimeInSecond 600 --fileName test 
>>>>>>> 4e3f3ccafe0e3c791ca4dbe137b32a7d252ae9d5

ping -n 2 127.0.0.1 > NUL

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --cl --port=8888 --characterID=0 --type=1 --occupation=1
<<<<<<< HEAD
=======

ping -n 2 127.0.0.1 > NUL

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --cl --port=8888 --characterID=4 --type=2 --occupation=1
>>>>>>> 4e3f3ccafe0e3c791ca4dbe137b32a7d252ae9d5
