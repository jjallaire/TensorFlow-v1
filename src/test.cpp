#include <Rcpp.h>
using namespace Rcpp;

#include "TensorFlowLib.hpp"

//' @export
// [[Rcpp::export]]
void testTF() {

   TensorFlowLib& TF = tensorFlowLib();

   TF_Status* pStatus = TF.NewStatus();
   TF.DeleteStatus(pStatus);

}
