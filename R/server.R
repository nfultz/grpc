
#' Start a gRPC server
#' 
#' @param impl an implementation of a proto service
#' @param host a hoststring in 'host:port' format
#' @return none
#' @importFrom methods selectMethod
#' @useDynLib grpc
#' @export
start_server <- function(impl, hoststring){
  
  server_functions <- lapply(impl, function(fn){
    descriptor <- P(fn[["RequestType"]]$proto)

    f <- structure(fn$f, 
                   RequestType  = fn[["RequestType"]],
                   ResponseType = fn[["ResponseType"]])
    
    function(x) serialize(f(read(descriptor, x)), NULL)
  })
  
  names(server_functions) <- vapply(impl, function(x)x$name, NA_character_)

  run(server_functions, hoststring)
  invisible(NULL)
}

#' @export
newResponse <- function(..., WFUN=sys.function(-1)){
  new(P(attr(WFUN, "ResponseType")$proto), ...)
}
