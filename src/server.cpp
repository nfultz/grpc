#include <Rcpp.h>
#include <grpc/grpc.h>
#include <grpc/impl/codegen/byte_buffer_reader.h>
#include "common.h"

using namespace Rcpp;


#define _INTERRUPT_CHECK_PERIOD_MS 1000

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

  RGRPC_LOG("Slice2Raw:\nn: " << n << "\nData: " << data);

  RawVector out(n);
  for(int i = 0; i < n; i++)
    out[i] = (unsigned char) data[i];
  // std::copy(data, data+n, out.begin());

  return out;
}



// [[Rcpp::export]]
List run(List target, CharacterVector hoststring) {

  bool done = false;
  // grpc_arg arg = {GRPC_ARG_STRING, "key", "value"};
  // grpc_channel_args channel_args = {1, &arg};

  RGRPC_LOG("Create Server");

  grpc_server* server = grpc_server_create(NULL /*&channel_args*/, 0);


  // create completion queue
  RGRPC_LOG("Creating Queue");
  grpc_completion_queue* queue = grpc_completion_queue_create_for_next(RESERVED); //todo
  grpc_server_register_completion_queue(server, queue, RESERVED);

  RGRPC_LOG("Bind");

  grpc_server_add_insecure_http2_port(server, hoststring[0]);

  // rock and roll
  Rcout << "Starting Server...";
  grpc_server_start(server);

  grpc_call *call;
  grpc_call_details details;
  grpc_metadata_array request_meta;
  grpc_call_error error;
  grpc_op ops[6];
  grpc_op *op;
  int was_cancelled = 2;


  grpc_byte_buffer *request_payload_recv;
  grpc_byte_buffer *response_payload;

  // init crap
  grpc_call_details_init(&details);
  grpc_metadata_array_init(&request_meta);



  Rcout << "\t[RUNNING]\n";

  // Copy pasted from node module...
  grpc_event event;
  do {
    memset(ops, 0, sizeof(ops));

    grpc_server_request_call(server, &call,
                             &details, &request_meta, queue, queue, NULL);
    // event = grpc_completion_queue_next(queue, gpr_inf_past(GPR_CLOCK_MONOTONIC),NULL);
    gpr_timespec c_increment = gpr_time_from_millis(_INTERRUPT_CHECK_PERIOD_MS, GPR_TIMESPAN);
    gpr_timespec c_timeout = gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), c_increment);

    event = grpc_completion_queue_next(queue, c_timeout, RESERVED);

    RGRPC_LOG("Event type: " << event.type);

    CharacterVector method =     sliceToChar(details.method);
    RGRPC_LOG("Event method: " << method);

    if (event.type == GRPC_OP_COMPLETE) {
      const char *error_message;
      if (event.success) {
        error_message = NULL;
      } else {
        error_message = "The async function encountered an error";
        Rcout << error_message << "\n";
        continue;
      }
      

      RGRPC_LOG("Processing event method: " << method );
      
      
      // CompleteTag(event.tag, error_message);
      // todo distpatch back to R here

      // var batch = {};
      // batch[grpc.opType.RECV_MESSAGE] = true;
      // call.startBatch();
      //

      //stolen from grpclb_test.cc


      // receive request for backends
      memset(ops, 0, sizeof(ops));
      op = ops;
      
      RGRPC_LOG("GRPC_OP_SEND_INITIAL_METADATA");
      op = ops;
      op->op = GRPC_OP_SEND_INITIAL_METADATA;
      op->data.send_initial_metadata.count = 0;
      op->data.send_initial_metadata.maybe_compression_level.is_set = false;
      op->flags = 0;
      op->reserved = NULL;
      op++;

      RGRPC_LOG("GRPC_OP_RECV_MESSAGE");
      op->op = GRPC_OP_RECV_MESSAGE;
      op->data.recv_message.recv_message = &request_payload_recv;
      op->flags = 0;
      op->reserved = NULL;
      op++;

      error = grpc_call_start_batch(call, ops, (size_t)(op - ops), NULL, NULL);
      // Rcout <<    (GRPC_CALL_OK == error ? "OK" : "Not OK") << "\n";
      // GPR_ASSERT(GRPC_CALL_OK == error);
      // CQ_EXPECT_COMPLETION(cqv, tag(202), 1);
      // cq_verify(cqv);
      grpc_completion_queue_next(queue, c_timeout, RESERVED); //actually does the work
      // gpr_log(GPR_INFO, "LB Server[%s](%s) after RECV_MSG", sf->servers_hostport,
      //         sf->balancer_name);

      // validate initial request.
      // Rcout << "Read Slice\n";

      grpc_byte_buffer_reader bbr;
      grpc_byte_buffer_reader_init(&bbr, request_payload_recv);
      grpc_slice request_payload_slice = grpc_byte_buffer_reader_readall(&bbr);

      RawVector request_payload_raw = sliceToRaw(request_payload_slice);

      RGRPC_LOG("...Slice2Raw Cleanup");
      grpc_slice_unref(request_payload_slice);
      grpc_byte_buffer_reader_destroy(&bbr);
      grpc_byte_buffer_destroy(request_payload_recv);


      // Done recving

      // op->op = GRPC_OP_RECV_CLOSE_ON_SERVER;
      // op->data.recv_close_on_server.cancelled = &was_cancelled;
      // op->flags = 0;
      // op->reserved = NULL;
      // op++;
      // Rcout << "Batch\n";
      // error = grpc_call_start_batch(call, ops, (size_t)(op - ops), NULL, NULL);
      // grpc_completion_queue_next(queue, c_timeout, RESERVED); //actually does the work


      // Fire callback
      Function callback = as<Rcpp::Function>(target[as<std::string>(method[0])]);

      RGRPC_LOG("callback()");
      RawVector response_payload_raw = callback(request_payload_raw);

      SEXP raw_ = response_payload_raw;

      // char buffer[50000] ;
      // // char buffer2[5000];
      int len = response_payload_raw.length();
      // for(int i = 0; i < len; i++){
      //   buffer[i] = (char) response_payload_raw[i];
      //   // sprintf(buffer2, "%02x ", buffer[i]);
      //   // Rcout << buffer2;
      // }
      // Rcout << "copied " << len <<  " bytes\n";
      // grpc_slice response_payload_slice = grpc_slice_from_copied_buffer(buffer, len);

      grpc_slice response_payload_slice = grpc_slice_from_copied_buffer((char*) RAW(raw_), len);

      memset(ops, 0, sizeof(ops));

      op = ops;
      op->op = GRPC_OP_RECV_CLOSE_ON_SERVER;
      op->data.recv_close_on_server.cancelled = &was_cancelled;
      op->flags = 0;
      op->reserved = NULL;
      op++;


      RGRPC_LOG("GRPC_OP_SEND_MESSAGE");
      response_payload = grpc_raw_byte_buffer_create(&response_payload_slice, 1);

      // op = ops;
      op->op = GRPC_OP_SEND_MESSAGE;
      op->data.send_message.send_message = response_payload;
      op->flags = 0;
      op->reserved = NULL;
      op++;



      RGRPC_LOG("GRPC_OP_SEND_STATUS_FROM_SERVER");
      // op = ops;
      op->op = GRPC_OP_SEND_STATUS_FROM_SERVER;
      op->data.send_status_from_server.trailing_metadata_count = 0;
      op->data.send_status_from_server.status = GRPC_STATUS_OK;
      grpc_slice status_details = grpc_slice_from_static_string("OK");
      op->data.send_status_from_server.status_details = &status_details;
      op->flags = 0;
      op->reserved = NULL;
      op++;

      RGRPC_LOG("Batch");
      error = grpc_call_start_batch(call, ops, (size_t)(op - ops), NULL, NULL);
      RGRPC_LOG("Hangup next");
      grpc_completion_queue_next(queue, c_timeout, RESERVED); //actually does the work
      // Rcout << "Hangup done...\n";

      // GPR_ASSERT(GRPC_CALL_OK == error);

      // Rcout << "response cleanup...\n";
      grpc_byte_buffer_destroy(response_payload);
      grpc_slice_unref(response_payload_slice);



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

    try{
      Rcpp::checkUserInterrupt();
    } catch (Rcpp::internal::InterruptedException ie){
      Rcout << "Stopping server...";
      done = true;
    }
    
  } while (!done);



  //shutdown
  RGRPC_LOG("Shutting down\n");
  grpc_server_shutdown_and_notify(server, queue, 0 /* tag */);
  grpc_server_cancel_all_calls(server);
  grpc_completion_queue_next(queue, gpr_inf_future(GPR_CLOCK_REALTIME), NULL);
  grpc_server_destroy(server);
  Rcout << "\t[STOPPED]\n";
  

  return List::create();
}
