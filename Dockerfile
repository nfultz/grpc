FROM rocker/r-base:latest
MAINTAINER Gergely Daroczi <gergely@system1.com>

ENV GRPC_RELEASE_TAG v1.4.5
ENV PROTOC_RELEASE_TAG v3.0.0

RUN apt-get update && apt-get install -y \
  build-essential autoconf libtool \
  libgflags-dev libgtest-dev clang libc++-dev \
  git curl \
  pkg-config \
  && apt-get clean && rm -rf /var/lib/apt/lists/

RUN git clone -b ${GRPC_RELEASE_TAG} https://github.com/grpc/grpc /var/local/git/grpc && \
    cd /var/local/git/grpc && git submodule update --init

RUN cd /var/local/git/grpc/third_party/protobuf && \
    git checkout ${PROTOC_RELEASE_TAG} && \
    ./autogen.sh && ./configure && \
    make && make install && make clean && ldconfig && \
    cd /var/local/git/grpc && \
    make && make install && make clean && ldconfig
