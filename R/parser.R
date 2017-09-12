#' Create stub object from protobuf spec
#' 
#' @param file the spec file
#' @return a stub data structure
#' @export
read_services <- function(file){
  SERVICE = "service"
  RPC = "rpc"
  RETURNS = "returns"
  STREAM = "stream"
  PACKAGE = "package"

  services <- list()
  pkg <- ""


  doServices <- function(i){
    service_name <- tokens[i+1]
    # services[[service_name]] <<- list()

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
    fn <- list(f=I)

    w <- "RequestType"

    while(tokens[i] != '}'){

      if(tokens[i] == '('){
        i <- i + 1
        isStream <- tokens[i] == STREAM
        if(isStream){
          i <- i + 1
        }

        fn[[w]] <- list(name=tokens[i], stream=isStream, proto=sprintf("%s.%s", pkg, tokens[i]))
        w <- "ResponseType"
      }

      i <- i + 1
    }
    services[[sprintf("/%s.%s/%s",pkg, service_name, rpc_name)]] <<- fn
    return(i)
  }

  readProtoFiles(file)

  lines <- readLines(file)

  tokens <- Filter(f=nchar, unlist(strsplit(lines, '(^//.*$|\\s+|(?=[{}();]))', perl=TRUE)))

  i <- 1
  while(i <= length(tokens)){
    if(tokens[i] == PACKAGE) {
      pkg <- tokens[i+1];
    }
    else if(tokens[i] == SERVICE){
      i <- doServices(i)
    }

    i <- i + 1
  }

  services
}
