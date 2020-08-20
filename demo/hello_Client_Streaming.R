#' Example gRPC client
#'
#' Sends a message with a name and returns a message greeting the name.
#' @references \url{https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld}

library(grpc)


spec <- system.file('examples/helloworld.proto', package = 'grpc')  
impl <- read_services(spec)

options(UseTLS = FALSE)
options(CertPath = "/home/shehab/Dropbox/gRPC/Certificates/")
options(AccessToken = "Bearer Token Shehab and Neal")
options(ClientDeadline = 10)

client <- grpc_client(impl, "localhost:50051")

everything <- client$SayEverything$build(name='Shehab')
queue <- client$SayEverything$call(everything)

for (message in queue) {
	print(as.list(message))
}
