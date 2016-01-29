
#ifndef __TENSOR_FLOW_LIB_HPP__
#define __TENSOR_FLOW_LIB_HPP__

#include "tensor_c_api.h"

// singleton
class TensorFlowLib ;
TensorFlowLib& tensorFlowLib();

// dynamically loaded interface to tensor flow shared library
class TensorFlowLib {

// initialization/destruction
public:
   bool isLoaded() { return pTensorFlowLib_ != NULL; }
   bool load(std::string* pError);
   ~TensorFlowLib();

// api
public:
   TF_Status* (*NewStatus)();
   void (*DeleteStatus)(TF_Status*);

// non-copyable singleton
private:
   friend TensorFlowLib& tensorFlowLib();
   TensorFlowLib();
   TensorFlowLib(const TensorFlowLib& other); // non construction-copyable
   TensorFlowLib& operator=(const TensorFlowLib&); // non copyable

private:
   void* pPythonLib_;
   void* pTensorFlowLib_;
};

#endif // __TENSOR_FLOW_LIB_HPP__
