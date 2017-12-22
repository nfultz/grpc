## Example gRPC client to score features sent in a Proto message

library(grpc)
library(RProtoBuf)

## reading the service definitions
spec <- system.file('examples/iris_classifier.proto', package = 'grpc')
impl <- read_services(spec)

## connection to the gRPC server
client <- grpc_client(impl, 'localhost:50051')

## define message to be sent to the gRPC server
msg <- client$Classify$build(Sepal_Length = 5, Sepal_Width = 2, Petal_Length = 1, Petal_Width = 0.3)

## score
res <- client$Classify$call(msg)

str(res)

## log results
library(futile.logger)
flog.info('Result: %s with %s probability', res$Species, res$Probability)
