#include <Rcpp.h>
#include <grpc/grpc.h>

using namespace Rcpp;

//' @export
// [[Rcpp::export]]
CharacterVector grpc_version() {
  return CharacterVector::create(grpc_version_string(), grpc_g_stands_for());
}
