#' Example gRPC service
#'
#' Reads a message with a name and returns a message greeting the name.
#' @references \url{https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld}
#' @importFrom RProtoBuf readProtoFiles read new serialize
helloworld <- function() {

    ## reading the service definitions
    readProtoFiles(system.file('examples/helloworld.proto', package = 'grpc'))

    ## defining service handlers
    handler <- list(
        '/helloworld.Greeter/SayHello' = function(x) {
            request  <- read(helloworld.HelloRequest, x)
            response <- new(helloworld.HelloReply, message = paste('Hello, ', request$name))
            serialize(response, NULL)
        })

    ## actually running the service handlers
    run(handler)

}
