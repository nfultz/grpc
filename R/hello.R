#' Example gRPC service
#'
#' Reads a message with a name and returns a message greeting the name.
#' @references \url{https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld}
#' @importFrom RProtoBuf readProtoFiles read new serialize P
helloworld <- function() {

    ## reading the service definitions
    spec <- system.file('examples/helloworld.proto', package = 'grpc')
    
    impl <- read_services(spec)
  
    ## defining service handlers
    SayHello <- grep('SayHello', names(impl))
    
    #hmmm slightly gross, write a helper
    impl[[SayHello]]$f <- function(request){
      ResponseType = P(attr(sys.function(), "ResponseType")$proto)
      response <- new(ResponseType, message = paste('Hello,', request$name))
      response      
    }
    

    ## actually running the service handlers
    start_server(impl)

}
