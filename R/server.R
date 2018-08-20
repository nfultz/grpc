
#' Start a gRPC server
#'
#' @param impl an implementation of a proto service
#' @param channel a channel string in 'host:port' format
#' @param on_start_hook R function with \code{port} argument to run when the gRPC service started
#' @return none
#' @importFrom methods selectMethod
#' @importFrom RProtoBuf P serialize read
#' @useDynLib grpc
#' @export
start_server <- function(impl, channel, on_start_hook = function(port)
    cat('gRPC service started on port', port, '\n')) {

  server_functions <- lapply(impl, function(fn){
    descriptor <- P(fn[["RequestType"]]$proto)

    f <- structure(fn$f,
                   RequestType  = fn[["RequestType"]],
                   ResponseType = fn[["ResponseType"]])

    function(x) serialize(f(read(descriptor, x)), NULL)
  })

  names(server_functions) <- vapply(impl, function(x)x$name, NA_character_)

  run(server_functions, channel, on_start_hook)
  invisible(NULL)
}


#' Construct a new ProtoBuf of ResponseType
#'
#' @param ... threaded through to the ProtoBuf constructor
#' @param WFUN A GRPC service method with RequestType and ResponseType attributes
#' @return
#'
#' @export
#' @importFrom RProtoBuf P new
newResponse <- function(..., WFUN=sys.function(-1)){
  new(P(attr(WFUN, "ResponseType")$proto), ...)
}
