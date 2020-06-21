#' Build a client handle 
#' @param stubs the stub of the service
#' @param channel what to connect to
#' @return client handle
#' @importFrom RProtoBuf P serialize read new
#' @export
grpc_client <- function(impl, channel)
{
  
  UseTLS <- getOption("UseTLS", default = TRUE)
  CertPath <- getOption("CertPath", default = NULL)
  AccessToken <- getOption("AccessToken", default = NULL)
  ClientDeadline <- getOption("ClientDeadline", default = 5)

  # lapply function is applied for operations on list objects and 
  # returns a list object of same length of original set.
  client_functions <- lapply(impl, 
    function(fn)
    {
        RequestDescriptor   <- P(fn[["RequestType"]]$proto)
        ResponseDescriptor  <- P(fn[["ResponseType"]]$proto)

        list(
            call = function(x, metadata=character(0)) 
            {   
                read(
                    ResponseDescriptor, 
                    fetch(channel, fn$name, serialize(x, NULL), metadata, UseTLS, CertPath, AccessToken, ClientDeadline)
                    )
            },
            
            build = function(...) 
            {
              new(RequestDescriptor, ...)
            }
        )
    }
  )
  
  client_functions
}
