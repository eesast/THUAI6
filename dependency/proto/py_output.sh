python3 -m grpc_tools.protoc -I. --python_out=. --pyi_out=. MessageType.proto
python3 -m grpc_tools.protoc -I. --python_out=. --pyi_out=. Message2Clients.proto
python3 -m grpc_tools.protoc -I. --python_out=. --pyi_out=. Message2Server.proto
python3 -m grpc_tools.protoc -I. --python_out=. --pyi_out=. --grpc_python_out=. Services.proto
chmod -R 755 ./
mv -f ./*.py ../../PyAPI/proto
mv -f ./*.pyi ../../PyAPI/proto
