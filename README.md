# grpc - An R library for RPC


Easily create [gRPC](https://github.com/grpc/grpc) clients and servers from protobuf descriptions to build distributed services. 

Copyright 2015 Google Inc, 2017 Neal Fultz


## Dependencies

  * grpc
  * protobuf
  * RProtoBuf

See `install` for my installation notes...


## Examples

  There are runnable examples in the `demo/` folder.
  
  To start a HelloWorld server:
  
    R -e 'demo(helloserver, "grpc")'
    
  To run a client against a running HelloWorld server:
  
    R -e 'demo(helloclient, "grpc")'
    
  Both are cross compatible with the Node, Python and C++ Greeter examples provided by the grpc library.
  
## Todo

  * Streaming services
  * Authentication and Encryption
  * Error handling
  * Docs
  
## Contributing