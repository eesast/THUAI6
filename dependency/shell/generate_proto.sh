#!/usr/bin/env bash

python -m pip install -r requirements.txt

mkdir -p proto

python -m grpc_tools.protoc -I../../dependency/proto/ --python_out=./proto --pyi_out=./proto MessageType.proto
python -m grpc_tools.protoc -I../../dependency/proto/ --python_out=./proto --pyi_out=./proto Message2Clients.proto
python -m grpc_tools.protoc -I../../dependency/proto/ --python_out=./proto --pyi_out=./proto Message2Server.proto
python -m grpc_tools.protoc -I../../dependency/proto/ --python_out=./proto --pyi_out=./proto --grpc_python_out=./proto Services.proto
