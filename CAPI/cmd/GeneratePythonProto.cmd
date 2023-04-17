@echo off

python -m pip install -r .\CAPI\python\requirements.txt

MKDIR .\CAPI\python\proto
echo generating python grpc files...

python -m grpc_tools.protoc -I.\CAPI\proto\ --python_out=.\CAPI\python\proto --pyi_out=.\CAPI\python\proto MessageType.proto
python -m grpc_tools.protoc -I.\CAPI\proto\ --python_out=.\CAPI\python\proto --pyi_out=.\CAPI\python\proto Message2Clients.proto
python -m grpc_tools.protoc -I.\CAPI\proto\ --python_out=.\CAPI\python\proto --pyi_out=.\CAPI\python\proto Message2Server.proto
python -m grpc_tools.protoc -I.\CAPI\proto\ --python_out=.\CAPI\python\proto --pyi_out=.\CAPI\python\proto --grpc_python_out=.\CAPI\python\proto Services.proto

pause
