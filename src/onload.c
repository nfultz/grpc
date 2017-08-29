#include <R.h>
#include <R_ext/Rdynload.h>
#include <grpc/grpc.h>

void R_init_grpc(DllInfo *info){
  
  grpc_init();
  Rprintf("Using GRPC version %s\n", grpc_version_string());
  
}


void R_unload_grpc(DllInfo *info){
  
  grpc_shutdown();
}