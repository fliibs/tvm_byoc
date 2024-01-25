#ifndef TVM_RUNTIME_CONTRIB_BYOC_C_BYOC_KERNEL_H_
#define TVM_RUNTIME_CONTRIB_BYOC_C_BYOC_KERNEL_H_

#include <tvm/runtime/c_runtime_api.h>
#include <tvm/runtime/logging.h>
#include <tvm/runtime/registry.h>

#include <vector>

namespace tvm {
namespace runtime {
namespace contrib {

enum FileType {
    BINARY,
    TEXT
};

extern "C" TVM_DLL void byoc_c_conv2d(float* data, float* weights, float* out, int p_N_, int p_C_, int p_H_,
                            int p_W_, int p_O_, int p_Kh_, int p_Kw_, int p_Sh_, int p_Sw_, 
                            int p_Ph0_, int p_Ph1_, int p_Pw0_, int p_Pw1_);

extern "C" TVM_DLL void byoc_c_bias_add(float* data, float* bias, float* out, int p_N_, int p_C_, int p_H_, int p_W_, int axis);

extern "C" TVM_DLL void byoc_c_relu(float* data, float* out, int p_N_, int p_C_, int p_H_, int p_W_);

extern "C" TVM_DLL void writeFloatToBinaryFile(float value, const std::string& filename);

extern "C" TVM_DLL void writeStringToTxtFile(std::string content, const std::string& filename);
}
}
}
#endif