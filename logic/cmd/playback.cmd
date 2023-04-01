@echo off

start cmd /k ..\Client\bin\Debug\net6.0-windows\Client.exe --cl --playbackFile D:\2_autumn\thuai6\THUAI6\logic\cmd\test.thuaipb

ping -n 2 127.0.0.1 > NUL