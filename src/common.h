#ifndef RGRPC_H
#define RGRPC_H

#include <Rcpp.h>
void debugLog(std::string message);

/* uncomment for debugging */
// #define RGRPC_DEBUG

#ifdef RGRPC_DEBUG
#define RGRPC_LOG(x) Rcout << x << "\n";
#else
#define RGRPC_LOG(x) 
#endif

#define RESERVED NULL

#endif


