#include <R_ext/Rdynload.h>
#include <grpc/grpc.h>

void R_init_grpc(DllInfo *info){
  grpc_init();
}


void R_unload_grpc(DllInfo *info){
  
  grpc_shutdown();
}