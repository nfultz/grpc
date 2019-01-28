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
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc grpc_base
cd grpc_base
git submodule update --init

## In gcc 8.2 I got an error which I could fix with (see https://github.com/grpc/grpc/issues/17781)
## with the following two export lines. (uncomment and run if you gat an error when calling make).
#export CFLAGS='-g -O2 -w' 
#export CXXFLAGS='-g -O2 -w'

make
sudo make install
sudo ldconfig
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
