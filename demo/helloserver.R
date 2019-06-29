#' Example gRPC service
#'
#' Reads a message with a name and returns a message greeting the name.
#' @references \url{https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld}

library(grpc)

## reading the service definitions
spec <- system.file('examples/helloworld.proto', package = 'grpc')

impl <- read_services(spec)

impl$SayHello$f <- function(request){
  newResponse(message = paste('Hello,', request$name))
}

impl$SayThanks$f <- function(request){
  newResponse(message = paste('Thanks,', request$name))
}

impl$SayBye$f <- function(request){
  newResponse(message = paste('Bye,', request$name))
}

## actually running the service handlers
start_server(impl, "0.0.0.0:50051")

