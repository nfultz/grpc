## Example gRPC service to score features sent in a Proto message

library(grpc)
library(futile.logger)
library(jsonlite)

## reading the service definitions
spec <- system.file('examples/iris_classifier.proto', package = 'grpc')

## build model for scoring (this is to be cached)
library(rpart)
fit <- rpart(Species ~ ., iris)

## define service definitions
impl <- read_services(spec)
impl$Classify$f <- function(request) {

    flog.info('Data received for scoring: %s', toJSON(as.list(request), auto_unbox = TRUE))

    ## fix colnames
    request <- as.list(request)
    names(request) <- sub('_', '.', names(request))

    ## try to score
    class <- tryCatch(predict(fit, newdata = request, type = 'class'),
                      error = function(e) e)

    ## return error code/message
    if (inherits(class, 'error')) {
        return(newResponse(Status = new(iris.Status, code = 1, message = e$message)))
    }

    ## return scors
    class <- as.character(class)
    p     <- predict(fit, newdata = request)[, class]

    flog.info('Predicted class: %s (p=%s)', class, p)

    ## return
    newResponse(Species = class, Probability = p)

}

## run the service handlers
start_server(impl, '0.0.0.0:50051')
