#!/usr/bin/env bash

python3 -m pip install -r requirements.txt

mkdir -p proto

python3 -m grpc_tools.protoc -I../../dependency/proto/ --python_out=./proto --pyi_out=./proto MessageType.proto
python3 -m grpc_tools.protoc -I../../dependency/proto/ --python_out=./proto --pyi_out=./proto Message2Clients.proto
python3 -m grpc_tools.protoc -I../../dependency/proto/ --python_out=./proto --pyi_out=./proto Message2Server.proto
python3 -m grpc_tools.protoc -I../../dependency/proto/ --python_out=./proto --pyi_out=./proto --grpc_python_out=./proto Services.proto
