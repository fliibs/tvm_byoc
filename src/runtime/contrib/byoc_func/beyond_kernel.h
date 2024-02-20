#ifndef TVM_RUNTIME_CONTRIB_BEYOND_KERNEL_H_
#define TVM_RUNTIME_CONTRIB_BEYOND_KERNEL_H_

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

extern "C" TVM_DLL void beyond_conv2d(float* data, float* weights, float* out, int p_N_, int p_C_, int p_H_,
                            int p_W_, int p_O_, int p_Kh_, int p_Kw_, int p_Sh_, int p_Sw_, 
                            int p_Ph0_, int p_Ph1_, int p_Pw0_, int p_Pw1_, std::string name);

extern "C" TVM_DLL void beyond_bias_add(float* data, float* bias, float* out, int p_N_, int p_C_, int p_H_, int p_W_, int axis, std::string name);

extern "C" TVM_DLL void beyond_relu(float* data, float* out, int p_N_, int p_C_, int p_H_, int p_W_, std::string name);

} // contrib 
} // runtime 
} // tvm
#endif