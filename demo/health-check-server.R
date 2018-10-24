## Example gRPC service + health check

library(grpc)
library(futile.logger)
library(jsonlite)
library(RProtoBuf)

## build model for scoring (saved to global environment)
library(rpart)
names(iris) <- tolower(sub('.', '_', names(iris), fixed = TRUE))
fit <- rpart(species ~ ., iris)

## reading the service definitions
impl <- c(
    read_services(system.file('examples/helloworld.proto', package = 'grpc')),
    read_services(system.file('examples/health_check.proto', package = 'grpc')))

impl$SayHello$f <- function(request){
  newResponse(message = paste('Hello,', request$name))
}

impl$Check$f <- function(request) {
    new(grpc.health.v1.HealthCheckResponse, status = 1)
}

## run the service handlers on an open port
start_server(impl, '0.0.0.0:0', hooks = list(
  run = function(params) {
    cat(params$port, file = '/tmp/health-check-server.port')
    flog.info('Server running on port %s', params$port)
  }
))
