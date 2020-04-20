#' Build a client handle
#' 
#' @param stubs the stub of the service
#' @param channel what to connect to
#' @return client handle
#' @importFrom RProtoBuf P serialize read new
#' @export
grpc_client <- function(impl, channel, client_deadline) {

  client_deadline <- getOption("client_deadline", default = 5)
  
  client_functions <- lapply(impl, function(fn)
    {
      RequestDescriptor <- P(fn[["RequestType"]]$proto)
      ResponseDescriptor <- P(fn[["ResponseType"]]$proto)
      
      list(
        call = function(x, metadata=character(0)) read(ResponseDescriptor, fetch(channel, fn$name, serialize(x, NULL), metadata, client_deadline)),
        build = function(...) new(RequestDescriptor, ...)
      )
    })
  
  
  
  client_functions
}
