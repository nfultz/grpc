# grpc - An R library for RPC

[![Build Status](https://travis-ci.org/nfultz/grpc.svg?branch=docker)](https://travis-ci.org/nfultz/grpc)

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
    
To run a client against a running HelloWorld server:
  
    R -e 'demo("helloclient", "grpc")'
    
Both are cross compatible with the Node, Python and C++ Greeter examples provided by the grpc library.

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
