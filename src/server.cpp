#include <Rcpp.h>
#include <grpc/grpc.h>
#include <grpc/impl/codegen/byte_buffer_reader.h>


using namespace Rcpp;


#define RESERVED NULL
#define _INTERRUPT_CHECK_PERIOD_MS 100000

CharacterVector sliceToChar(grpc_slice slice){

  char* data =
  const_cast<char *>(reinterpret_cast<const char *>(
      GRPC_SLICE_START_PTR(slice)));

  CharacterVector out(1);
  out[0] = data;

  return out;
}

RawVector sliceToRaw(grpc_slice slice){

  int n = GRPC_SLICE_LENGTH(slice);

  char* data =
    const_cast<char *>(reinterpret_cast<const char *>(
        GRPC_SLICE_START_PTR(slice)));

  Rcout << "Slice2Raw:\nn: " << n << "\nData: " << data <<"\n";

  RawVector out(n);
  for(int i = 0; i < n; i++)
    out[i] = (unsigned char) data[i];
  // std::copy(data, data+n, out.begin());

  return out;
}



//' @export
// [[Rcpp::export]]
List run(List target) {

  // grpc_arg arg = {GRPC_ARG_STRING, "key", "value"};
  // grpc_channel_args channel_args = {1, &arg};

  Rcout << "Create Server\n";

  grpc_server* server = grpc_server_create(NULL /*&channel_args*/, 0);


  // create completion queue
  Rcout << "Creating Queue\n";
  grpc_completion_queue* queue = grpc_completion_queue_create_for_next(RESERVED); //todo
  grpc_server_register_completion_queue(server, queue, RESERVED);

  Rcout << "Bind\n";

  grpc_server_add_insecure_http2_port(server, "0.0.0.0:50051");

  // rock and roll
  Rcout << "Starting Server\n";
  grpc_server_start(server);

  grpc_call *call;
  grpc_call_details details;
  grpc_metadata_array request_meta;
  grpc_call_error error;
  grpc_op ops[6];
  grpc_op *op;

  grpc_byte_buffer *request_payload_recv;
  grpc_byte_buffer *response_payload;

  // init crap
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
        Rcout << "Event method: " << sliceToChar(details.method) << "\n";

    if (event.type == GRPC_OP_COMPLETE) {
      const char *error_message;
      if (event.success) {
        error_message = NULL;
      } else {
        error_message = "The async function encountered an error";
      }
      // CompleteTag(event.tag, error_message);
      // todo distpatch back to R here

      // var batch = {};
      // batch[grpc.opType.RECV_MESSAGE] = true;
      // call.startBatch();
      //

      //stolen from grpclb_test.cc


      // receive request for backends
      op = ops;
      op->op = GRPC_OP_RECV_MESSAGE;
      op->data.recv_message.recv_message = &request_payload_recv;
      op->flags = 0;
      op->reserved = NULL;
      op++;
      Rcout << "GRPC_OP_RECV_MESSAGE\n";

      error = grpc_call_start_batch(call, ops, (size_t)(op - ops), NULL, NULL);
      // Rcout <<    (GRPC_CALL_OK == error ? "OK" : "Not OK") << "\n";
      // GPR_ASSERT(GRPC_CALL_OK == error);
      // CQ_EXPECT_COMPLETION(cqv, tag(202), 1);
      // cq_verify(cqv);
      grpc_completion_queue_next(queue, c_timeout, RESERVED); //actually does the work
      // gpr_log(GPR_INFO, "LB Server[%s](%s) after RECV_MSG", sf->servers_hostport,
      //         sf->balancer_name);

      // validate initial request.
      Rcout << "Read Slice\n";

      grpc_byte_buffer_reader bbr;
      grpc_byte_buffer_reader_init(&bbr, request_payload_recv);
      grpc_slice request_payload_slice = grpc_byte_buffer_reader_readall(&bbr);

      RawVector request_payload_raw = sliceToRaw(request_payload_slice);

      Rcout << "Slice2Raw Cleanup\n";
      grpc_slice_unref(request_payload_slice);
      grpc_byte_buffer_reader_destroy(&bbr);
      grpc_byte_buffer_destroy(request_payload_recv);


      // Fire callback
      Function callback = as<Function>(target[0]);

      Rcout << "callback";
      callback(request_payload_raw);


      //
      //
      // grpc_byte_buffer* recv_message = result.read;
      // RawVector* recv_serialized = new RawVector(recv_message);
      //
      // void* message = as<Function>(recv_serialized)("here");
      //
      //
      // message.grpcWriteFlags = flags;
      // end_batch[grpc.opType.SEND_MESSAGE] = message;
      // end_batch[grpc.opType.SEND_STATUS_FROM_SERVER] = status;
      // call.startBatch(end_batch, function (){});
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
