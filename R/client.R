#' Build a client handle
#' 
#' @param stubs the stub of the service
#' @param channel what to connect to
#' @return client handle
#' @importFrom RProtoBuf P serialize read new
#' @export
grpc_client <- function(impl, channel) {
  
  
  client_functions <- lapply(impl, function(fn)
    {
      RequestDescriptor <- P(fn[["RequestType"]]$proto)
      ResponseDescriptor <- P(fn[["ResponseType"]]$proto)
      
      list(
        call = function(x) read(ResponseDescriptor, fetch(channel, fn$name, serialize(x, NULL))),
        callWithMetadata = function(x, metadata) read(ResponseDescriptor, fetchWithMetadata(channel, fn$name, serialize(x, NULL), metadata)),
        build = function(...) new(RequestDescriptor, ...)
      )
    })
  
  
  
  client_functions
}