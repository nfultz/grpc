#' Example gRPC client
#'
#' Sends a message with a name and returns a message greeting the name.
#' @references \url{https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld}

library(grpc)


spec <- system.file('examples/helloworld.proto', package = 'grpc')  
impl <- read_services(spec)
client <- grpc_client(impl, "localhost:50051")


for(who in c("Neal", "Gergely")){
    hello <- client$SayHello$build(name=who)
    message <- client$SayHello$call(hello)
    
    print(message)
    print(as.list(message))

    thanks <- client$SayThanks$build(name=who)
    message <- client$SayThanks$callWithMetadata(thanks, c("key1", "val1"))
    
    print(message)
    print(as.list(message))

    bye <- client$SayBye$build(name=who)
    message <- client$SayBye$call(bye)
    
    print(message)
    print(as.list(message))
}
