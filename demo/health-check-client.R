library(grpc)
library(RProtoBuf)
library(futile.logger)

## reading the service definitions
impl <- c(
    read_services(system.file('examples/helloworld.proto', package = 'grpc')),
    read_services(system.file('examples/health_check.proto', package = 'grpc')))

## connection to the gRPC server
port   <- readLines('/tmp/health-check-server.port', warn = FALSE)
client <- grpc_client(impl, paste('localhost', port, sep = ':'))

## check if service is online
res <- client$Check$call(client$Check$build())
flog.info(as.character(res))

## actual gRPC call
res <- client$SayHello$call(client$SayHello$build(name = 'Gergely'))
flog.info(as.character(res))
