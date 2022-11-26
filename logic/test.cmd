@echo off

start cmd /k .\Server\bin\Debug\net6.0\Server.exe

start cmd /k .\ClientTest\bin\Debug\net6.0\ClientTest.exe 0 1

start cmd /k .\ClientTest\bin\Debug\net6.0\ClientTest.exe 1 2
