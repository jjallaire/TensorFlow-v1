
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
   // status
   TF_Status* (*NewStatus)();
   void (*DeleteStatus)(TF_Status* s);
   void (*SetStatus)(TF_Status* s, TF_Code code, const char* msg);
   TF_Code (*GetCode)(const TF_Status* s);
   const char* (*Message)(const TF_Status* s);

   // tensors
   TF_Tensor* (*NewTensor)(TF_DataType, long long* dims, int num_dims,
                           void* data, size_t len,
                           void (*deallocator)(void* data,size_t len,void* arg),
                           void* deallocator_arg);
   void (*DeleteTensor)(TF_Tensor*);
   TF_DataType (*TensorType)(const TF_Tensor*);
   int (*NumDims)(const TF_Tensor*);
   long long (*Dim)(const TF_Tensor* tensor, int dim_index);
   size_t (*TensorByteSize)(const TF_Tensor*);
   void* (*TensorData)(const TF_Tensor*);

   // session options
   TF_SessionOptions* (*NewSessionOptions)();
   void (*SetTarget)(TF_SessionOptions* options, const char* target);
   void (*SetConfig)(TF_SessionOptions* options, const void* proto,
                     size_t proto_len, TF_Status* status);
   void (*DeleteSessionOptions)(TF_SessionOptions*);

   // session
   TF_Session* (*NewSession)(const TF_SessionOptions*, TF_Status* status);
   void (*CloseSession)(TF_Session*, TF_Status* status);
   void (*DeleteSession)(TF_Session*, TF_Status* status);
   void (*ExtendGraph)(TF_Session*, const void* proto, size_t proto_len,
                       TF_Status*);

   // execution
   void (*Run)(TF_Session*,
               // Input tensors
               const char** input_names, TF_Tensor** inputs, int ninputs,
               // Output tensors
               const char** output_tensor_names, TF_Tensor** outputs,
               int noutputs,
               // Target nodes
               const char** target_node_names, int ntargets,
               // Output status
               TF_Status*);

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
