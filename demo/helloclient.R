helloclient <- function(){
  
  
  spec <- system.file('examples/helloworld.proto', package = 'grpc')  
  impl <- read_services(spec)
  client <- grpc_client(impl, "localhost:50051")
  
  neal = client$SayHello$build(name="Neal")
  
  hello <- client$SayHello$call(neal)
  
  print(hello)
  print(as.list(hello))
  
}