## TODO
read_services <- function(file){
  SERVICE = "service"
  RPC = "rpc"
  RETURNS = "returns"
  STREAM = "stream"

  services <- list()

  doServices <- function(i){
    service_name <- tokens[i+1]
    services[[service_name]] <<- list()

    while(tokens[i] != '}') {
      if(tokens[i] == RPC){
        i <- doRPC(i, service_name)
      }
      i <- i + 1
    }

    return(i)
  }

  doRPC <- function(i, service_name) {
    rpc_name = tokens[i+1]
    fn <- I

    w <- "RequestType"

    while(tokens[i] != '}'){

      if(tokens[i] == '('){
        i <- i + 1
        isStream <- tokens[i] == STREAM
        if(isStream){
          i <- i + 1
        }

        attributes(fn)[[w]] <- list(name=tokens[i], stream=isStream)
        w <- "ResponseType"
      }

      i <- i + 1
    }
    services[[service_name]][[rpc_name]] <<- fn
    return(i)
  }


  lines <- readLines(file)

  tokens <- strsplit(lines, '(^//.*$|\\s+|(?=[{}();]))', perl=TRUE) %>% unlist %>% Filter(f=nchar)

  i <- 1
  while(i <= length(tokens)){
    if(tokens[i] == SERVICE){
      i <- doServices(i)
    }

    i <- i + 1
  }

  services
}
