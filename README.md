# grpc

An **R** library for [**GRPC**](https://grpc.io/) a high-performance, open-source universal RPC framework.

## Installation - Debian

### Pre-requisites

The following is copied from [gRPC C++ - Building from source](https://github.com/grpc/grpc/blob/master/BUILDING.md)
```shell
sudo apt-get install build-essential autoconf libtool pkg-config
## If you plan to build from source and run tests, install the following as well:
sudo apt-get install libgflags-dev libgtest-dev
sudo apt-get install clang libc++-dev
```

### Download and Install grpc
```shell
export GRPC_INSTALL_DIR=$HOME/.local
mkdir -p $GRPC_INSTALL_DIR
export PATH="$GRPC_INSTALL_DIR/bin:$PATH"

sudo apt install -y cmake

LATEST_VER=$(curl -L "https://api.github.com/repos/grpc/grpc/releases/latest" | grep -Po '"tag_name": "\K.*?(?=")')
git clone --recurse-submodules -b $LATEST_VER https://github.com/grpc/grpc grpc_base

cd grpc_base
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$GRPC_INSTALL_DIR \
      ../..
make -j4
sudo make install
popd

mkdir -p third_party/abseil-cpp/cmake/build
pushd third_party/abseil-cpp/cmake/build
cmake -DCMAKE_INSTALL_PREFIX=$GRPC_INSTALL_DIR \
      -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
      ../..
make -j4
sudo make install
popd
```

# Original 

[![Build Status](https://travis-ci.org/nfultz/grpc.svg)](https://travis-ci.org/nfultz/grpc)

Easily create [gRPC](https://github.com/grpc/grpc) clients and servers from protobuf descriptions to build distributed services. 

Copyright 2015 Google Inc, 2017 Neal Fultz


## Dependencies

  * grpc
  * protobuf
  * RProtoBuf

See `install` for my installation notes...


## Examples

There are runnable examples in the `demo/` folder.

### Hello, World!

To start a HelloWorld server:
  
    R -e 'demo("helloserver", "grpc")'

Or with much more detailed logging:
  
    R -e 'library(futile.logger); flog.threshold(TRACE); demo("helloserver", "grpc")'

To run a client against a running HelloWorld server:
  
    R -e 'demo("helloclient", "grpc")'
    
Both are cross compatible with the Node, Python and C++ Greeter examples provided by the grpc library.

### Health check

This server implements the above service along with the standard [GRPC Health Checking Protocol](https://github.com/grpc/grpc/blob/master/doc/health-checking.md):

    R -e 'demo("health-check-server", "grpc")'

The client runs a health-check then calls the Hello, World! method once:

    R -e 'demo("health-check-client", "grpc")'

Please check the sources of the server to see how to bundle services defined in multiple `proto` files.

### Live scoring

There's a simple trained on the `iris` dataset and making that available for scoring via a gRPC service:

    R -e 'demo("iris-server", "grpc")'

An example client to this service from R:

    R -e 'demo("iris-client", "grpc")'

## Todo

  * Streaming services
  * Authentication and Encryption
  * Error handling
  * Docs
  
## Contributing
