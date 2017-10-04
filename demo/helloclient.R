#' Example gRPC client
#'
#' Reads a message with a name and returns a message greeting the name.
#' @references \url{https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld}

spec <- system.file('examples/helloworld.proto', package = 'grpc')  
impl <- read_services(spec)
client <- grpc_client(impl, "localhost:50051")

neal = client$SayHello$build(name="Neal")

hello <- client$SayHello$call(neal)

print(hello)
print(as.list(hello))
