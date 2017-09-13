
#' Start a gRPC server
#' 
#' @param impl an implementation of a proto service
#' @return none
#' @importFrom methods selectMethod
#' @useDynLib grpc
#' @export
start_server <- function(impl){
  
  server_functions <- lapply(impl, function(fn){
    descriptor <- P(fn[["RequestType"]]$proto)

    f <- structure(fn$f, 
                   RequestType  = fn[["RequestType"]],
                   ResponseType = fn[["ResponseType"]])
    
    function(x) serialize(f(read(descriptor, x)), NULL)
  })

  run(server_functions)
  invisible(NULL)
}