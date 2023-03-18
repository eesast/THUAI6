FROM ubuntu:22.04
MAINTAINER eesast.com
WORKDIR /usr/local
RUN mkdir /usr/local/PlayerCode
#安装主要工具
RUN apt-get update && apt-get install --no-install-recommends -y gcc g++ make wget ca-certificates cmake autoconf automake libtool curl unzip git
#安装grpc
RUN git clone -b v1.46.3 --depth 1 --shallow-submodules  https://gitee.com/mirrors/grpc.git
RUN wget -P . https://cloud.tsinghua.edu.cn/f/1f2713efd9e44255abd6/?dl=1
RUN mv 'index.html?dl=1' third_party.tar.gz
RUN cd grpc
RUN rm -rf third_party
RUN mv ../third_party.tar.gz .
RUN tar -zxvf third_party.tar.gz
RUN mkdir -p cmake/build
RUN pushd cmake/build
RUN cmake -DgRPC_INSTALL=ON \
-DgRPC_BUILD_TESTS=OFF \
../..
RUN make -j$(nproc)
RUN make install
RUN popd

#安装protobuf
RUN cd /usr/local
RUN git clone https://gitee.com/mirrors/protobuf_source.git ./protobuf
RUN cd protobuf
RUN git checkout 3.22.1
RUN ./autogen.sh
RUN ./configure
RUN make -j$(nproc)
RUN make install
RUN ldconfig

COPY ./CAPI /usr/local/PlayerCode
RUN cd /usr/local/PlayerCode/CAPI
RUN cmake CMakeLists.txt
RUN make
