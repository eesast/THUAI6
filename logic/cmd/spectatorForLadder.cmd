@echo off

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --port 8889 --characterId 2080 --ip thuai6.eesast.com

ping -n 2 127.0.0.1 > NUL