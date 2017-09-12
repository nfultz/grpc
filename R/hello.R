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

    readProtoFiles(system.file('examples/helloworld.proto', package = 'grpc'))
    handler <- list(

        '/helloworld.Greeter/SayHello' = function(x) {

            x <- read(helloworld.HelloRequest, x)
            ## message("x is")
            ## cat(writeLines(as.character(x)))

            y <- new(helloworld.HelloReply, message = paste('Hello, ', x$name))
            ## str(y)
            ## message("y is")
            ## cat(writeLines(as.character(y)))

            y2 <- serialize(y, NULL)
            ## str(y2)
            message("y2 is")
            print(y2)
            ## y3 <- read(helloworld.HelloReply, y2)
            ## str(y3)
            ## message("y3 is")
            ## cat(writeLines(as.character(y3)))
            return(y2[])

        })

    run(handler)

}
