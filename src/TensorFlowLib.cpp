
#include <Rcpp.h>
using namespace Rcpp;

#include <dlfcn.h>

#include "TensorFlowLib.hpp"

namespace {

bool dlError(const std::string& fallback, std::string* pError)
{
   const char* dlmsg = ::dlerror();
   if (dlmsg != NULL)
      *pError =  dlmsg;
   else
      *pError = fallback;
   return false;
}

bool loadLibrary(const std::string& libraryPath,
                 int mode,
                 void** ppLib,
                 std::string* pError)
{
   *ppLib = ::dlopen(libraryPath.c_str(), mode);
   if (*ppLib == NULL)
      return dlError("Error loading " + libraryPath, pError);
   else
      return true;
}

bool loadSymbol(void* pLib,
                const char* name,
                void** ppSymbol,
                std::string* pError)
{
   *ppSymbol = ::dlsym(pLib, name);
   if (*ppSymbol == NULL)
      return dlError("Error looking up " + std::string(name), pError);
   else
      return true;
}

bool closeLibrary(void* pLib, const std::string& name, std::string* pError)
{
   if (::dlclose(pLib) != 0)
      return dlError("Error closing " + name, pError);
   else
      return true;
}

} // anonymous namespace

#define LOAD_TENSOR_FLOW_SYMBOL(name)                                         \
   if (!loadSymbol(pTensorFlowLib_, "TF_" #name, (void**)&name, pError))      \
      return false;

// singleton accessor which initializes on demand
TensorFlowLib& tensorFlowLib()
{
   static TensorFlowLib instance;
   if (!instance.isLoaded()) {
      std::string err;
      if (!instance.load(&err))
         stop(err);
   }
   return instance;
}

TensorFlowLib::TensorFlowLib()
   : pPythonLib_(NULL), pTensorFlowLib_(NULL)
{
}

TensorFlowLib::~TensorFlowLib()
{
   try
   {
      std::string err;
      if (!closeLibrary(pTensorFlowLib_, "libtensorflow", &err))
         std::cerr << err << std::endl;
      if (!closeLibrary(pPythonLib_, "libpython", &err))
         std::cerr << err << std::endl;
   }
   catch(...)
   {
   }
}

// load the library
bool TensorFlowLib::load(std::string* pError)
{
   // determine the paths of libraries to load
   Rcpp::Function sharedLibraryPaths("sharedLibraryPaths", "TensorFlow");
   Rcpp::List paths = sharedLibraryPaths();
   std::string pythonPath = as<std::string>(paths["python"]);
   std::string tensorflowPath = as<std::string>(paths["tensorflow"]);

   // dynamically load the python library
   if (!loadLibrary(pythonPath,
                    RTLD_NOW | RTLD_GLOBAL,
                    &pPythonLib_,
                    pError)) {
      return false;
   }

   // dynamically load the tensor flow library
   if (!loadLibrary(tensorflowPath,
                    RTLD_NOW | RTLD_GLOBAL,
                    &pTensorFlowLib_,
                    pError)) {
      return false;
   }

   // lookup symbols
   LOAD_TENSOR_FLOW_SYMBOL(NewStatus);
   LOAD_TENSOR_FLOW_SYMBOL(DeleteStatus);
   LOAD_TENSOR_FLOW_SYMBOL(SetStatus);
   LOAD_TENSOR_FLOW_SYMBOL(GetCode);
   LOAD_TENSOR_FLOW_SYMBOL(Message);
   LOAD_TENSOR_FLOW_SYMBOL(NewTensor);
   LOAD_TENSOR_FLOW_SYMBOL(DeleteTensor);
   LOAD_TENSOR_FLOW_SYMBOL(TensorType);
   LOAD_TENSOR_FLOW_SYMBOL(NumDims);
   LOAD_TENSOR_FLOW_SYMBOL(Dim);
   LOAD_TENSOR_FLOW_SYMBOL(TensorByteSize);
   LOAD_TENSOR_FLOW_SYMBOL(TensorData);
   LOAD_TENSOR_FLOW_SYMBOL(NewSessionOptions);
   LOAD_TENSOR_FLOW_SYMBOL(SetTarget);
   LOAD_TENSOR_FLOW_SYMBOL(SetConfig);
   LOAD_TENSOR_FLOW_SYMBOL(DeleteSessionOptions);
   LOAD_TENSOR_FLOW_SYMBOL(NewSession);
   LOAD_TENSOR_FLOW_SYMBOL(CloseSession);
   LOAD_TENSOR_FLOW_SYMBOL(DeleteSession);
   LOAD_TENSOR_FLOW_SYMBOL(ExtendGraph);
   LOAD_TENSOR_FLOW_SYMBOL(Run);

   return true;
}
