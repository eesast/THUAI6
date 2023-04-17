@echo off

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --playbackFile .\test.thuaipb --playbackSpeed 2

ping -n 2 127.0.0.1 > NUL