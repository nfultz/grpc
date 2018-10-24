#ifndef RGRPC_H
#define RGRPC_H

/* uncomment for debugging */
// #define RGRPC_DEBUG

#ifdef RGRPC_DEBUG
#define RGRPC_LOG(x) Rcout << x << std::endl;
#else
#define RGRPC_LOG(x) 
#endif

#define RESERVED NULL

#endif


