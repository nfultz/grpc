#' Client and server implementaiton of Google's Remote Procedure Call framework
#'
#' See the \code{\link{helloworld}} example.
#' @name grpc
#' @docType package
#' @docType package
#' @useDynLib grpc
#' @import Rcpp
NULL

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
