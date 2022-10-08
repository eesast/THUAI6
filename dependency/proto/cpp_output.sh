protoc Message2Clients.proto --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin`
protoc Message2Clients.proto --cpp_out=.
protoc MessageType.proto --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin`
protoc MessageType.proto --cpp_out=.
protoc Message2Server.proto --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin`
protoc Message2Server.proto --cpp_out=.
chmod -R 755 ./
./format.sh
mv -f ./*.h ../../CAPI/proto
mv -f ./*.cc ../../CAPI/proto