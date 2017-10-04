#' Example gRPC client
#'
#' Sends a message with a name and returns a message greeting the name.
#' @references \url{https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld}

library(grpc)


spec <- system.file('examples/helloworld.proto', package = 'grpc')  
impl <- read_services(spec)
client <- grpc_client(impl, "localhost:50051")


for(who in c("Neal", "Gergely")){
    who <- client$SayHello$build(name=who)
    
    hello <- client$SayHello$call(who)
    
    print(hello)
    print(as.list(hello))
}
