#ifndef CLIENTLIBRARIES_H
#define CLIENTLIBRARIES_H

#include <Rcpp.h>
#include <grpc/grpc.h>
#include <grpc/impl/codegen/byte_buffer_reader.h>
#include <grpc/impl/codegen/log.h>
#include <grpc/slice.h>
#include <grpc/grpc_security.h>
#include <grpc/support/alloc.h>
#include <grpc/support/string_util.h>

#include "common.h"

#define GPR_ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))

struct files {
    std::string CAcert, clientKey, clientCert;
};

#endif
