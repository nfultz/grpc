#include <Rcpp.h>
#include <grpc/grpc.h>

using namespace Rcpp;


#define RESERVED NULL
#define _INTERRUPT_CHECK_PERIOD_MS 100000

//' @export
// [[Rcpp::export]]
List run(List target) {

  // grpc_arg arg = {GRPC_ARG_STRING, "key", "value"};
  // grpc_channel_args channel_args = {1, &arg};

  Rcout << "Create Server\n";

  grpc_server* server = grpc_server_create(NULL /*&channel_args*/, 0);


  // create completion queue
  Rcout << "Creating Queue\n";
  grpc_completion_queue* queue = grpc_completion_queue_create(RESERVED); //todo
  grpc_server_register_completion_queue(server, queue, RESERVED);

  Rcout << "Bind\n";

  grpc_server_add_insecure_http2_port(server, "0.0.0.0:50051");

  // rock and roll
  Rcout << "Starting Server\n";
  grpc_server_start(server);

  grpc_call *call;
  grpc_call_details details;
  grpc_metadata_array request_meta;
  grpc_call_details_init(&details);
  grpc_metadata_array_init(&request_meta);



  Rcout << "Entering Event Loop\n";

  // Copy pasted from node module...
  grpc_event event;
  do {
    grpc_server_request_call(server, &call,
                             &details, &request_meta, queue, queue, NULL);
    // event = grpc_completion_queue_next(queue, gpr_inf_past(GPR_CLOCK_MONOTONIC),NULL);
    gpr_timespec c_increment = gpr_time_from_millis(_INTERRUPT_CHECK_PERIOD_MS, GPR_TIMESPAN);
    gpr_timespec c_timeout = gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), c_increment);

    event = grpc_completion_queue_next(queue, c_timeout, RESERVED);

        Rcout << "Event type: " << event.type << "\n";

    if (event.type == GRPC_OP_COMPLETE) {
      const char *error_message;
      if (event.success) {
        error_message = NULL;
      } else {
        error_message = "The async function encountered an error";
      }
      // CompleteTag(event.tag, error_message);
      // todo distpatch back to R here
      as<Function>(target[0])("here");
    }

  } while (1);



  //shutdown
  Rcout << "Shutting down\n";
  grpc_server_shutdown_and_notify(server, queue, 0 /* tag */);
  grpc_server_cancel_all_calls(server);
  grpc_completion_queue_pluck(queue, NULL,
                              gpr_inf_future(GPR_CLOCK_REALTIME), NULL);
  grpc_server_destroy(server);


  return List::create();
}
