#' Start a gRPC server
#'
#' @param impl an implementation of a proto service
#' @param channel a channel string in 'host:port' format
#' @param hooks list of R function(s) with \code{params} argument as a list to be run at different parts of the C++ calls. Supported hooks: \code{server_create}, \code{queue_create}, \code{bind} (when \code{params$port} becomes available), \code{server_start}, \code{run}
#' @return none
#' @importFrom methods selectMethod
#' @importFrom RProtoBuf P serialize read
#' @useDynLib grpc
#' @export
#' @seealso \code{\link{grpc_default_hooks}}
start_server <- function(impl, channel, hooks = grpc_default_hooks()) {

  server_functions <- lapply(impl, function(fn){
    descriptor <- P(fn[["RequestType"]]$proto)

    f <- structure(fn$f,
                   RequestType  = fn[["RequestType"]],
                   ResponseType = fn[["ResponseType"]])

    function(x) serialize(f(read(descriptor, x)), NULL)
  })

  names(server_functions) <- vapply(impl, function(x)x$name, NA_character_)

  run(server_functions, channel, hooks)
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
