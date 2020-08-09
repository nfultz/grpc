#include <iostream>
#include <fstream>
#include "Client_Libraries.h"

using namespace Rcpp;

static void* tag(intptr_t i) {
  return (void*)i;
}

RawVector sliceToRaw2(grpc_slice slice) {

  int n = GRPC_SLICE_LENGTH(slice);

  char* data = grpc_slice_to_c_string(slice);

  RGRPC_LOG("Slice2Raw:\nn: " << n << "\nData: " << data);

  RawVector out(n);
  for(int i = 0; i < n; i++)
    out[i] = (unsigned char) data[i];

  return out;
}

static std::string get_file_contents(const char *fpath) {
  std::ifstream finstream(fpath);

  std::string contents(
	  (std::istreambuf_iterator<char>(finstream)),
	  std::istreambuf_iterator<char>()
	  );

  return contents;
}

const char* getOauth2AccessToken(const char* tokenValue) {

  if(tokenValue == nullptr)
    stop("Access token value is not defined");

  const char* accessToken(tokenValue + sizeof("Bearer ") - 1);
  
  return accessToken;
}

grpc_channel_credentials* getClientCredentials(const char* path, const char* tokenValue, bool isMetadataAttached){

  files PEMfiles;

  PEMfiles.CAcert = 
    get_file_contents(((std::string)path + "ca-cert.pem").c_str());
  PEMfiles.clientKey = 
    get_file_contents(((std::string)path + "client-key.pem").c_str());
  PEMfiles.clientCert = 
    get_file_contents(((std::string)path + "client-cert.pem").c_str());
  
  grpc_ssl_pem_key_cert_pair signed_client_key_cert_pair =
    {(PEMfiles.clientKey).c_str(), (PEMfiles.clientCert).c_str()};

  grpc_channel_credentials* creds = grpc_ssl_credentials_create(
    (PEMfiles.CAcert).c_str(), &signed_client_key_cert_pair, nullptr, nullptr);

  if(isMetadataAttached)
    return creds;

  grpc_call_credentials* oauth2Creds = 
    grpc_access_token_credentials_create(getOauth2AccessToken(tokenValue), nullptr);
  
  grpc_channel_credentials* credsTLSOauth =
    grpc_composite_channel_credentials_create(creds, oauth2Creds, nullptr);
    
  return credsTLSOauth;
}

grpc_channel* createChannel(bool useTLS, const char* server ,const char* path, 
                            const char* tokenValue, bool isMetadataAttached) {
  
  grpc_channel* channel;

  if(useTLS) {
    grpc_channel_credentials* client_creds = getClientCredentials(path, tokenValue, isMetadataAttached);
    channel = grpc_secure_channel_create(client_creds, server, NULL, nullptr);
  }
  
  else {
    channel = grpc_insecure_channel_create(server, NULL, nullptr);
  }

  return channel;
}

// [[Rcpp::export]]
RawVector fetch(CharacterVector server, CharacterVector method, RawVector requestArg, CharacterVector metadata, 
                bool useTLS, CharacterVector certPath, CharacterVector tokenValue, int clientDeadline) {

  //RGRPC_LOG("RawVector Here = " << method);
  //method = "/helloworld.Greeter/SayThanks";

  bool isMetadataAttached = 
    (metadata.length() > 0) ? true : false;

  grpc_channel* channel = 
    createChannel(useTLS, server[0], certPath[0], tokenValue[0], isMetadataAttached);

  grpc_completion_queue* cq = 
    grpc_completion_queue_create_for_next(nullptr);

  const grpc_slice method_slice = 
    grpc_slice_from_copied_string(method[0]);
  
  const grpc_slice server_slice = 
    grpc_slice_from_copied_string(server[0]);
  const grpc_slice *sp = &server_slice;

  //Get client deadline in milliseconds
  int timeout = clientDeadline * 1000;
  gpr_timespec deadline =
    gpr_time_add(gpr_now(GPR_CLOCK_REALTIME), gpr_time_from_millis(timeout, GPR_TIMESPAN));

  RGRPC_LOG("Create Call");
  grpc_call* c = grpc_channel_create_call(
                      channel, NULL, GRPC_PROPAGATE_DEFAULTS, 
                      cq, method_slice, sp, deadline, NULL);

  RGRPC_LOG("Making ops here");
  grpc_op ops[6];
  grpc_op* op;

  int metadata_length = metadata.length() / 2;
  grpc_metadata meta_c[metadata_length];

  for(int i = 0; i < metadata_length; i++) {
    meta_c[i] = {grpc_slice_from_static_string(metadata[i * 2]),
                 grpc_slice_from_static_string(metadata[i * 2 + 1]),
                 0,
                 {{nullptr, nullptr, nullptr, nullptr}}};
  }


  grpc_metadata_array initial_metadata_recv;
  grpc_metadata_array trailing_metadata_recv;
  grpc_metadata_array request_metadata_recv;
  grpc_byte_buffer *response_payload_recv;
  grpc_call_details call_details;
  grpc_status_code status;
  grpc_call_error error;
  grpc_event event;
  grpc_slice details;

  grpc_metadata_array_init(&initial_metadata_recv);
  grpc_metadata_array_init(&trailing_metadata_recv);
  grpc_metadata_array_init(&request_metadata_recv);
  grpc_call_details_init(&call_details);

  SEXP raw_ = requestArg;
  int len = requestArg.length();
  grpc_slice request_payload_slice = grpc_slice_from_copied_buffer((char*) RAW(raw_), len);

  grpc_byte_buffer *request_payload =
    grpc_raw_byte_buffer_create(&request_payload_slice, 1);

  memset(ops, 0, sizeof(ops));
  
  // 1
  op = ops;
  op->op = GRPC_OP_SEND_INITIAL_METADATA;
  op->data.send_initial_metadata.count = metadata_length;
  
  if (metadata_length > 0) {
    op->data.send_initial_metadata.metadata = meta_c;
    RGRPC_LOG("Initial Metadata = " << meta_c)
  }

  else {
    op->data.send_initial_metadata.maybe_compression_level.is_set = false;
  }

  op->flags = 0;
  op->reserved = NULL;
  op++;

  // 2
  op->op = GRPC_OP_SEND_MESSAGE;
  op->data.send_message.send_message = request_payload;
  op->flags = 0;
  op->reserved = NULL;
  op++;

  // 3
  op->op = GRPC_OP_SEND_CLOSE_FROM_CLIENT;
  op->flags = 0;
  op->reserved = NULL;
  op++;

  // 4
  op->op = GRPC_OP_RECV_INITIAL_METADATA;
  op->data.recv_initial_metadata.recv_initial_metadata = &initial_metadata_recv;
  op->flags = 0;
  op->reserved = NULL;
  op++;

  // 5
  op->op = GRPC_OP_RECV_MESSAGE;
  op->data.recv_message.recv_message = &response_payload_recv;
  op->flags = 0;
  op->reserved = NULL;
  op++;

  // 6
  op->op = GRPC_OP_RECV_STATUS_ON_CLIENT;
  op->data.recv_status_on_client.trailing_metadata = &trailing_metadata_recv;
  op->data.recv_status_on_client.status = &status;
  op->data.recv_status_on_client.status_details = &details;
  op->flags = 0;
  op->reserved = NULL;
  op++;

  RGRPC_LOG("Starting batch...");

  error = grpc_call_start_batch(c, ops, (size_t)(op - ops), tag(1), NULL);

  if (error != GRPC_CALL_OK) { // 0
    stop("gRPC c++ call start failed");
  }

  RGRPC_LOG("Set Event...");
  RGRPC_LOG("Eveeeeent Typeeeeee = " << event.type);
  event = grpc_completion_queue_next(cq, deadline, nullptr); //actually does the work
  RGRPC_LOG("Eveeeeent Typeeeeee = " << event.type);
  RGRPC_LOG("Did it reach here?");

  if (event.type == GRPC_QUEUE_TIMEOUT) { // 1
    stop("gRPC c++ call timeout");
  }

  if (event.success == 0) {
    stop("gRPC c++ call error");
  }

  if(status == GRPC_STATUS_UNAUTHENTICATED) {
    stop("The request does not have valid authentication credentials");
  }

  if (!response_payload_recv) {
    stop("No response from the gRPC server");
  }

  // Call Header, followed by optional Initial-Metadata, followed by zero or more Payload Messages
  RGRPC_LOG("Read response Slice");
  grpc_byte_buffer_reader bbr;

  try {
    grpc_byte_buffer_reader_init(&bbr, response_payload_recv);
  }

  catch (std::exception& e) {
    Rcout << "Segfault" << e.what() << std::endl;
    stop("Segfault in C++");
  }

  grpc_slice response_payload_slice = grpc_byte_buffer_reader_readall(&bbr);
  RawVector response_payload_raw = sliceToRaw2(response_payload_slice);

  RGRPC_LOG("response_payload_raw = " << response_payload_raw);
  
  //Streaming Starts Here

  grpc_op ops_1[6];

  memset(ops, 0, sizeof(ops_1));
  
  // 4
  op->op = GRPC_OP_RECV_INITIAL_METADATA;
  op->data.recv_initial_metadata.recv_initial_metadata = &initial_metadata_recv;
  op->flags = 0;
  op->reserved = NULL;
  op++;

  // 5
  op->op = GRPC_OP_RECV_MESSAGE;
  op->data.recv_message.recv_message = &response_payload_recv;
  op->flags = 0;
  op->reserved = NULL;
  op++;

  // 6
  op->op = GRPC_OP_RECV_STATUS_ON_CLIENT;
  op->data.recv_status_on_client.trailing_metadata = &trailing_metadata_recv;
  op->data.recv_status_on_client.status = &status;
  op->data.recv_status_on_client.status_details = &details;
  op->flags = 0;
  op->reserved = NULL;
  op++;

  error = grpc_call_start_batch(c, ops_1, (size_t)(op - ops_1), tag(1), NULL);
  grpc_event event_1;

  RGRPC_LOG("Event Type = " << event_1.type);
  event_1 = grpc_completion_queue_next(cq, deadline, nullptr); //actually does the work
  RGRPC_LOG("Event Type = " << event_1.type);

  RGRPC_LOG("Read response Slice");
  grpc_byte_buffer_reader bbr_1;

  try {
    grpc_byte_buffer_reader_init(&bbr_1, response_payload_recv);
  }

  catch (std::exception& e) {
    Rcout << "Segfault" << e.what() << std::endl;
    stop("Segfault in C++");
  }

  response_payload_slice = grpc_byte_buffer_reader_readall(&bbr_1);
  response_payload_raw = sliceToRaw2(response_payload_slice);

  RGRPC_LOG("response_payload_raw = " << response_payload_raw);

  //Streaming Ends Here

  RGRPC_LOG("Cleanup");

  grpc_completion_queue_shutdown(cq);
  grpc_completion_queue_destroy(cq);
  cq = NULL;
  grpc_channel_destroy(channel);
  channel = NULL;

  grpc_slice_unref(details);
  grpc_metadata_array_destroy(&initial_metadata_recv);
  grpc_metadata_array_destroy(&trailing_metadata_recv);
  grpc_metadata_array_destroy(&request_metadata_recv);
  grpc_call_details_destroy(&call_details);

  grpc_byte_buffer_destroy(request_payload);
  grpc_byte_buffer_destroy(response_payload_recv);
 
  return response_payload_raw;
  //return 0;
}
