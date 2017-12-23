## Example gRPC service to score features sent in a Proto message

library(grpc)
library(futile.logger)
library(jsonlite)
library(RProtoBuf)

## reading the service definitions
spec <- system.file('examples/iris_classifier.proto', package = 'grpc')

## build model for scoring (saved to global environment)
library(rpart)
names(iris) <- tolower(sub('.', '_', names(iris), fixed = TRUE))
fit <- rpart(species ~ ., iris)

## define service definitions
impl <- read_services(spec)

impl$Classify$f <- function(request) {

    request <- as.list(request)

    flog.info('Data received for scoring: %s', toJSON(request, auto_unbox = TRUE))

    ## try to score
    response <- tryCatch({

          for (v in attr(terms(fit), "term.labels")) {
              if (request[[v]] > 0) next else {
                  stop('Negative ', v, ' provided')
              }
          }

          scores <- predict(fit, newdata = request)
          i <- which.max(scores)
          cls <- attr(fit, "ylevels")[i]
          p <- scores[, i]

          flog.info('Predicted class: %s (p=%5.4f)', cls, p)
          new(iris.Class, species = cls, probability = p)
      },
      error = function(e) {
          new(iris.Class, status = new(iris.Status, code = 1, message = e$message))
      })
    response
}

## run the service handlers
start_server(impl, '0.0.0.0:50051')
