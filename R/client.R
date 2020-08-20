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

  client_functions <- lapply(impl, 
  function(fn)
  {
	  RequestDescriptor   <- P(fn[["RequestType"]]$proto)
	  ResponseDescriptor  <- P(fn[["ResponseType"]]$proto)
	  stream_bool <- fn[["ResponseType"]]$stream
	  message <- list(
		  call = function(x, metadata=character(0)) 
		  {
			  if(stream_bool) {
				  i <- 1
				  List_messages <- list()
				  while(1) {
					  message <- read(
						  ResponseDescriptor, 
						  fetch(channel, fn$name, serialize(x, NULL), metadata, UseTLS, CertPath, AccessToken, ClientDeadline)
						  )
						  
					  if(as.list(message) == '')
						break;
						
					  List_messages[[i]] <- message
					  i <- i + 1
					  }
				}

			  else {
				List_messages <- read(
					ResponseDescriptor, 
					fetch(channel, fn$name, serialize(x, NULL), metadata, UseTLS, CertPath, AccessToken, ClientDeadline)
					)
				}
				
				List_messages
            },
            
            build = function(...) 
            {
              new(RequestDescriptor, ...)
            }
        )
		
		message
	}
  )
  
  client_functions
}
