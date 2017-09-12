#include <Rcpp.h>
#include <grpc/grpc.h>

using namespace Rcpp;

// This is a simple function using Rcpp that creates an R list
// containing a character vector and a numeric vector.
//
// Learn more about how to use Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//
// and browse examples of code using Rcpp at:
//
//   http://gallery.rcpp.org/
//

//' @export
// [[Rcpp::export]]
CharacterVector grpc_version() {
  return CharacterVector::create(grpc_version_string(), grpc_g_stands_for());
}
