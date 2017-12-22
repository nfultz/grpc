## Example gRPC client to score features sent in a Proto message

library(grpc)
library(RProtoBuf)
library(futile.logger)

## reading the service definitions
spec <- system.file('examples/iris_classifier.proto', package = 'grpc')
impl <- read_services(spec)

## connection to the gRPC server
client <- grpc_client(impl, 'localhost:50051')

## define message to be sent to the gRPC server
msg <- client$Classify$build(sepal_length = 5, sepal_width = 2, petal_length = 1, petal_width = 0.3)

## score
res <- client$Classify$call(msg)

## log results
flog.info('Result: %s with %s probability', res$species, res$probability)

## score a number of items
df <- iris[sample(1:150, 25), ]
for (n in seq_len(nrow(df))) {

    ## define message to be sent to the gRPC server
    msg <- client$Classify$build(
                               sepal_length = df[n, 'Sepal.Length'],
                               sepal_width  = df[n, 'Sepal.Width'],
                               petal_length = df[n, 'Petal.Length'],
                               petal_width  = df[n, 'Petal.Width'])

    ## score
    res <- client$Classify$call(msg)

    ## log results
    flog.info('Result: %s with %s probability', res$species, res$probability)

}

