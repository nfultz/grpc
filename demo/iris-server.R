## Example gRPC service to score features sent in a Proto message

library(grpc)
library(futile.logger)
library(jsonlite)

## reading the service definitions
spec <- system.file('examples/iris_classifier.proto', package = 'grpc')

## build model for scoring (this is to be cached)
library(rpart)
names(iris) <- tolower(sub('.', '_', names(iris), fixed = TRUE))
fit <- rpart(species ~ ., iris)

## define service definitions
impl <- read_services(spec)
impl$Classify$f <- function(request) {

    flog.info('Data received for scoring: %s', toJSON(as.list(request), auto_unbox = TRUE))

    ## try to score
    request <- as.list(request)
    class   <- tryCatch(predict(fit, newdata = request, type = 'class'),
                        error = function(e) e)

    ## return error code/message on failure
    if (inherits(class, 'error')) {
        flog.error(class$message)
        return(newResponse(status = new(iris.Status, code = 1, message = class$message)))
    }

    ## return score otherwise
    class <- as.character(class)
    p     <- predict(fit, newdata = request)[, class]

    flog.info('Predicted class: %s (p=%s)', class, p)
    newResponse(species = class, probability = p)

}

## run the service handlers
start_server(impl, '0.0.0.0:50051')
