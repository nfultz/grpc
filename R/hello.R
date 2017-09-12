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

iris_classifier <- function() {

    ## reading the service definitions
    readProtoFiles(system.file('examples/iris_classifier.proto', package = 'grpc'))

    ## build model for scoring (this is to be cached)
    library(rpart)
    fit <- rpart(Species ~ ., iris)

    ## defining service handlers
    handler <- list(
        '/iris.Classifier/Classify' = function(x) {

            ## parse request
            request <- read(iris.Features, x)

            ## transform into an R object for scoring
            request <- as.list(request)

            ## fix colnames
            names(request) <- sub('_', '.', names(request))

            ## score
            class <- as.character(predict(fit, newdata = request, type = 'class'))

            ## return
            response <- new(iris.Class, Species = class)
            serialize(response, NULL)

        })

    ## actually running the service handlers
    run(handler)

}
