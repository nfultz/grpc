#include <Rcpp.h>
using namespace Rcpp;

void debugLog(std::string message){
  Environment env = Environment::namespace_env("grpc");
  LogicalVector debug = env["debug"];
  if (debug[0] == TRUE) {
    Rcout << "cpplog " << message << std::endl;
  }
}
