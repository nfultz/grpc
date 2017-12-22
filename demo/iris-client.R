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
msg <- client$Classify$build(Sepal_Length = 5, Sepal_Width = 2, Petal_Length = 1, Petal_Width = 0.3)

## score
res <- client$Classify$call(msg)

## log results
flog.info('Result: %s with %s probability', res$Species, res$Probability)

## score a number of items
df <- iris[sample(1:150, 25), ]

for (n in seq_len(nrow(df))) {

    ## define message to be sent to the gRPC server
    msg <- client$Classify$build(
                               Sepal_Length = df[n, 'Sepal.Length'],
                               Sepal_Width  = df[n, 'Sepal.Width'],
                               Petal_Length = df[n, 'Petal.Length'],
                               Petal_Width  = df[n, 'Petal.Width'])

    ## score
    res <- client$Classify$call(msg)

    ## log results
    flog.info('Result: %s with %s probability', res$Species, res$Probability)

}
