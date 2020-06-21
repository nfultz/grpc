#include <iostream>
#include <fstream>
#include "Server_Libraries.h"

using namespace Rcpp;

CharacterVector sliceToChar(grpc_slice slice) {

  char* data = grpc_slice_to_c_string(slice);
  CharacterVector out(1);
  out[0] = data;

  return out;
}

RawVector sliceToRaw(grpc_slice slice) {

  int n = GRPC_SLICE_LENGTH(slice);
  char* data = grpc_slice_to_c_string(slice);
  RGRPC_LOG("Slice2Raw:\nn: " << n << "\nData: " << data);
  RawVector out(n);

  for(int i = 0; i < n; i++)
    out[i] = (unsigned char) data[i];

  return out;
}

void runFunctionIfProvided(List hooks, std::string hook, List params) {

  if (hooks.containsElementNamed(hook.c_str())) {
    RGRPC_LOG("[HOOK " << hook << "] found and starting");
    Function hookFunction = hooks[hook];
    hookFunction(params);
    RGRPC_LOG("[HOOK " << hook << "] finished");
  }

  else {
    RGRPC_LOG("[HOOK " << hook << "] not found");
  }
}

static std::string get_file_contents(const char *fpath) {
  
  std::ifstream finstream(fpath);

  std::string contents(
	  (std::istreambuf_iterator<char>(finstream)),
	  std::istreambuf_iterator<char>()
	  );

  return contents;
}

grpc_server_credentials* Get_TLS_Credentials(const char* path) {

  std::string ca_cert_pem = 
    get_file_contents(((std::string)path + "ca-cert.pem").c_str());
  std::string server_key_pem = 
    get_file_contents(((std::string)path + "server-key.pem").c_str());
  std::string server_cert_pem = 
    get_file_contents(((std::string)path + "server-cert.pem").c_str());

  grpc_ssl_pem_key_cert_pair pem_cert_key_pair =
    {server_key_pem.c_str(), server_cert_pem.c_str()};

  grpc_server_credentials* Creds =
    grpc_ssl_server_credentials_create_ex(
      ca_cert_pem.c_str(),
      &pem_cert_key_pair,
      1,
      GRPC_SSL_REQUEST_CLIENT_CERTIFICATE_AND_VERIFY,
      nullptr);

  return Creds;
}

bool validateOauth2(grpc_metadata_array metadataArray, const char* token) {

  Authentication_Token Oauth2;

  int Index = metadataArray.count - 2;

  std::string TokenValue = (std::string)token;
  std::string TokenKey = "authorization";

  if (metadataArray.count > 1) {

    Oauth2.Value = 
      (std::string)grpc_slice_to_c_string(metadataArray.metadata[Index].value);

    Oauth2.Key = 
      (std::string)grpc_slice_to_c_string(metadataArray.metadata[Index].key);

    if(Oauth2.Key.compare(TokenKey) != 0 ||
       Oauth2.Value.compare(TokenValue) != 0) {
      return false;
    }
  }

  return true;
}

int createPort(bool useTLS, grpc_server* server, const char* hoststring, const char* CertPath) {

  int port;

  if(useTLS) {
    grpc_server_credentials* credentials = Get_TLS_Credentials(CertPath);
    port = grpc_server_add_secure_http2_port(server, hoststring, credentials);
    grpc_server_credentials_release(credentials);
  }
  
  else {
    port = grpc_server_add_insecure_http2_port(server, hoststring);
  }

  return port;
}

// [[Rcpp::export]]
List run(List target, CharacterVector hoststring, List hooks, bool useTLS, CharacterVector CertPath, 
         CharacterVector AccessToken) {

  List params = List::create();

  bool done = false;

  RGRPC_LOG("Create Server");
  grpc_server* server = 
    grpc_server_create(nullptr, nullptr);
  runFunctionIfProvided(hooks, "server_create", params);

  RGRPC_LOG("Creating Queue");
  grpc_completion_queue* queue = 
    grpc_completion_queue_create_for_next(nullptr);
  grpc_server_register_completion_queue(server, queue, nullptr);
  runFunctionIfProvided(hooks, "queue_create", params);

  RGRPC_LOG("Bind");
  int port = 
    createPort(useTLS, server, hoststring[0], CertPath[0]);
  params["port"] = port;
  runFunctionIfProvided(hooks, "bind", params);
  
  RGRPC_LOG("Starting Server on port " << port);
  grpc_server_start(server);
  runFunctionIfProvided(hooks, "server_start", params);


  grpc_call *call;
  grpc_call_details details;
  grpc_metadata_array request_meta;
  grpc_call_error error;
  grpc_op ops[6];
  grpc_op *op;
  int was_cancelled = 2;

  grpc_byte_buffer *request_payload_recv;
  grpc_byte_buffer *response_payload;
  grpc_byte_buffer *response_authentication;


  grpc_call_details_init(&details);
  grpc_metadata_array_init(&request_meta);
  runFunctionIfProvided(hooks, "run", params);

  RGRPC_LOG("[RUNNING]");

  // Copy pasted from node module...
  grpc_event event;

  do {

    memset(ops, 0, sizeof(ops));

    grpc_server_request_call(server, &call,
                             &details, &request_meta, queue, queue, NULL);

    gpr_timespec c_increment = gpr_time_from_millis(_INTERRUPT_CHECK_PERIOD_MS, GPR_TIMESPAN);
    gpr_timespec c_timeout = gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), c_increment);

    event = grpc_completion_queue_next(queue, c_timeout, RESERVED);

    RGRPC_LOG("Event type: " << event.type);
    CharacterVector method =     sliceToChar(details.method);

    RGRPC_LOG("Event method: " << method);
    params["event_method"] = method;

    if (event.type == GRPC_OP_COMPLETE) {
      
      const char *error_message;
      
      if (event.success) {
        error_message = NULL;
      } 
      
      else {
        error_message = "The async function encountered an error";
        RGRPC_LOG(error_message);
        continue;
      }

      RGRPC_LOG("Processing event method: " << method );
      runFunctionIfProvided(hooks, "event_received", params);

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

      grpc_completion_queue_next(queue, c_timeout, RESERVED); //actually does the work

      grpc_byte_buffer_reader bbr;
      grpc_byte_buffer_reader_init(&bbr, request_payload_recv);
      grpc_slice request_payload_slice = grpc_byte_buffer_reader_readall(&bbr);

      RawVector request_payload_raw = sliceToRaw(request_payload_slice);

      RGRPC_LOG("...Slice2Raw Cleanup");
      grpc_slice_unref(request_payload_slice);
      grpc_byte_buffer_reader_destroy(&bbr);
      grpc_byte_buffer_destroy(request_payload_recv);

      grpc_status_code status_code = GRPC_STATUS_UNKNOWN;
      char const *status_details_string = "Unknown error";

      // Fire callback
      if (target.containsElementNamed(method[0])) {

        RGRPC_LOG("Method found: " << method[0]);
        Function callback = as<Rcpp::Function>(target[as<std::string>(method[0])]);

        try {

          RawVector response_payload_raw = callback(request_payload_raw);
          runFunctionIfProvided(hooks, "event_processed", params);
          RGRPC_LOG("callback() success");

          status_code = GRPC_STATUS_OK;
          status_details_string = "OK";

          int len = response_payload_raw.length();
          SEXP raw_ = response_payload_raw;
          grpc_slice response_payload_slice = grpc_slice_from_copied_buffer((char*) RAW(raw_), len);
          response_payload = grpc_raw_byte_buffer_create(&response_payload_slice, 1);
          grpc_slice_unref(response_payload_slice);

        } 
        
        catch(...) {
          RGRPC_LOG("callback() failed");
        }

      } 
      
      else {

        RGRPC_LOG("Method not found: " << method[0]);
        status_code = GRPC_STATUS_UNIMPLEMENTED;
        status_details_string = "Method not implemented";

      }

      if(!validateOauth2(request_meta, AccessToken[0])) {
        status_code = GRPC_STATUS_UNAUTHENTICATED;
      }

      memset(ops, 0, sizeof(ops));

      op = ops;
      op->op = GRPC_OP_RECV_CLOSE_ON_SERVER;
      op->data.recv_close_on_server.cancelled = &was_cancelled;
      op->flags = 0;
      op->reserved = NULL;
      op++;

      if (status_code == GRPC_STATUS_OK) {

        RGRPC_LOG("GRPC_OP_SEND_MESSAGE");

        op->op = GRPC_OP_SEND_MESSAGE;
        op->data.send_message.send_message = response_payload;
        op->flags = 0;
        op->reserved = NULL;
        op++;
      }
      
      RGRPC_LOG("GRPC_OP_SEND_STATUS_FROM_SERVER");
      op->op = GRPC_OP_SEND_STATUS_FROM_SERVER;
      op->data.send_status_from_server.trailing_metadata_count = 0;

      op->data.send_status_from_server.status = status_code;
      grpc_slice status_details = grpc_slice_from_static_string(status_details_string);

      op->data.send_status_from_server.status_details = &status_details;
      op->flags = 0;
      op->reserved = NULL;
      op++;

      RGRPC_LOG("Batch");
      error = grpc_call_start_batch(call, ops, (size_t)(op - ops), NULL, NULL);
      RGRPC_LOG("Hangup next");
      grpc_completion_queue_next(queue, c_timeout, RESERVED); //actually does the work

      grpc_byte_buffer_destroy(response_payload);

    }

    try {
      Rcpp::checkUserInterrupt();
    } 
    
    catch (Rcpp::internal::InterruptedException ie) {
      RGRPC_LOG("Stopping server...");
      done = true;
    }

  } while (!done);

  //shutdown
  RGRPC_LOG("Shutting down\n");
  runFunctionIfProvided(hooks, "shutdown", params);
  grpc_server_shutdown_and_notify(server, queue, 0 /* tag */);
  grpc_server_cancel_all_calls(server);
  grpc_completion_queue_next(queue, gpr_inf_future(GPR_CLOCK_REALTIME), NULL);
  grpc_server_destroy(server);
  RGRPC_LOG("[STOPPED]");
  runFunctionIfProvided(hooks, "stopped", params);

  return List::create();
}
