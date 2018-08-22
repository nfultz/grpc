#' Provides a list of logging functions to be passed to gRPC server
#'
#' You may want to override this with custom functions, eg registering the gRPC service in Hashicorp's consul.io or similar after \code{server_start} etc.
#' @return \code{list}
#' @export
#' @importFrom futile.logger flog.trace flog.debug flog.info
grpc_default_hooks <- function() {
  list(
    server_create = function(params) {
      flog.trace('gRPC server created')
    },
    queue_create = function(params) {
      flog.trace('Completion queue created and registered')
    },
    bind = function(params) {
      flog.debug(paste('gRPC service will listen on port', params$port))
    },
    server_start = function(params) {
      flog.trace(paste('gRPC service started on port', params$port))
    },
    run = function(params) {
      flog.info(paste('gRPC service is now listening on port', params$port))
    },
    event_received = function(params) {
      flog.trace(paste('event received', params$event_method))
    },
    event_processed = function(params) {
      flog.trace('event processed')
    },
    shutdown = function(params) {
      flog.info('gRPC service is going to shut down')
    },
    stopped = function(params) {
      flog.debug('gRPC service stopped')
    }
  )
}
