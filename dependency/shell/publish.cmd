@ECHO OFF
CD %~dp0
dotnet publish "../../Logic/Server/Server.csproj" -c Release -r linux-x64 --self-contained true
dotnet publish "../../Logic/Server/Server.csproj" -c Release -r win-x64 --self-contained true
dotnet publish "../../Logic/Server/Server.csproj" -c Debug -r win-x64 --self-contained true
dotnet publish "../../Logic/Server/Server.csproj" -c Debug -r linux-x64 --self-contained true
dotnet publish "../../Logic/Client/Client.csproj" -c Release -r win-x64 --self-contained true
PAUSE
