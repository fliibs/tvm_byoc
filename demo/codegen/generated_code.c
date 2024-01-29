;#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <tvm/runtime/c_runtime_api.h>
#include <tvm/runtime/c_backend_api.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/packed_func.h>
#include <byoc_c/byoc_c_kernel.h>
using namespace tvm::runtime;
using namespace tvm::runtime::contrib;
#ifdef __cplusplus
tvm::runtime::Array<tvm::runtime::NDArray> tvmgen_default_byoc_c_main_5_consts;
#endif

void tvmgen_default_byoc_c_main_5_(float* byoc_c_5_i0, float* out0) {
  ICHECK(!tvmgen_default_byoc_c_main_5_consts.empty()) << "C source module hasn't been initialized.";

  byoc_c_conv2d(byoc_c_5_i0, (float*)(tvmgen_default_byoc_c_main_5_consts[0]->data), out0, 1, 1, 208, 208, 3, 3, 3, 1, 1, 0, 0, 0, 0, "conv2d");

}

int tvmgen_default_byoc_c_main_5_wrapper_(DLTensor* arg0,
	DLTensor* out0) {
  tvmgen_default_byoc_c_main_5_((float*)(arg0->data),
  (float*)(out0->data));
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif
TVM_DLL int32_t tvmgen_default_byoc_c_main_5(TVMValue* args, int* type_code, int num_args, TVMValue* out_value, int* out_type_code) {
  DLTensor* arg0 = (DLTensor*)(((TVMValue*)args)[0].v_handle);
  DLTensor* ret1 = (DLTensor*)(((TVMValue*)args)[1].v_handle);
  tvmgen_default_byoc_c_main_5_wrapper_(arg0,ret1);
  return 0;
}
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
int tvmgen_default_byoc_c_main_5_init_wrapper_(tvm::runtime::Array<tvm::runtime::NDArray> arr) {
  tvmgen_default_byoc_c_main_5_consts = arr;
return 0;
}

TVM_DLL_EXPORT_TYPED_FUNC(__init_tvmgen_default_byoc_c_main_5, tvmgen_default_byoc_c_main_5_init_wrapper_);

#endif
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <tvm/runtime/c_runtime_api.h>
#include <tvm/runtime/c_backend_api.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/packed_func.h>
#include <byoc_c/byoc_c_kernel.h>
using namespace tvm::runtime;
using namespace tvm::runtime::contrib;
#ifdef __cplusplus
tvm::runtime::Array<tvm::runtime::NDArray> tvmgen_default_byoc_c_main_7_consts;
#endif

void tvmgen_default_byoc_c_main_7_(float* byoc_c_7_i0, float* out0) {
  ICHECK(!tvmgen_default_byoc_c_main_7_consts.empty()) << "C source module hasn't been initialized.";

  byoc_c_bias_add(byoc_c_7_i0, (float*)(tvmgen_default_byoc_c_main_7_consts[0]->data), out0, 1, 3, 206, 206, 1, "bias_add");

}

int tvmgen_default_byoc_c_main_7_wrapper_(DLTensor* arg0,
	DLTensor* out0) {
  tvmgen_default_byoc_c_main_7_((float*)(arg0->data),
  (float*)(out0->data));
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif
TVM_DLL int32_t tvmgen_default_byoc_c_main_7(TVMValue* args, int* type_code, int num_args, TVMValue* out_value, int* out_type_code) {
  DLTensor* arg0 = (DLTensor*)(((TVMValue*)args)[0].v_handle);
  DLTensor* ret1 = (DLTensor*)(((TVMValue*)args)[1].v_handle);
  tvmgen_default_byoc_c_main_7_wrapper_(arg0,ret1);
  return 0;
}
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
int tvmgen_default_byoc_c_main_7_init_wrapper_(tvm::runtime::Array<tvm::runtime::NDArray> arr) {
  tvmgen_default_byoc_c_main_7_consts = arr;
return 0;
}

TVM_DLL_EXPORT_TYPED_FUNC(__init_tvmgen_default_byoc_c_main_7, tvmgen_default_byoc_c_main_7_init_wrapper_);

#endif
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <tvm/runtime/c_runtime_api.h>
#include <tvm/runtime/c_backend_api.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/packed_func.h>
#include <byoc_c/byoc_c_kernel.h>
using namespace tvm::runtime;
using namespace tvm::runtime::contrib;
void tvmgen_default_byoc_c_main_4_(float* byoc_c_4_i0, float* out0) {
  byoc_c_relu(byoc_c_4_i0, out0, 1, 1, 208, 208, "relu");

}

int tvmgen_default_byoc_c_main_4_wrapper_(DLTensor* arg0,
	DLTensor* out0) {
  tvmgen_default_byoc_c_main_4_((float*)(arg0->data),
  (float*)(out0->data));
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif
TVM_DLL int32_t tvmgen_default_byoc_c_main_4(TVMValue* args, int* type_code, int num_args, TVMValue* out_value, int* out_type_code) {
  DLTensor* arg0 = (DLTensor*)(((TVMValue*)args)[0].v_handle);
  DLTensor* ret1 = (DLTensor*)(((TVMValue*)args)[1].v_handle);
  tvmgen_default_byoc_c_main_4_wrapper_(arg0,ret1);
  return 0;
}
#ifdef __cplusplus
}
#endif
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <tvm/runtime/c_runtime_api.h>
#include <tvm/runtime/c_backend_api.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/packed_func.h>
#include <byoc_c/byoc_c_kernel.h>
using namespace tvm::runtime;
using namespace tvm::runtime::contrib;
#ifdef __cplusplus
tvm::runtime::Array<tvm::runtime::NDArray> tvmgen_default_byoc_c_main_2_consts;
#endif

void tvmgen_default_byoc_c_main_2_(float* byoc_c_2_i0, float* out0) {
  ICHECK(!tvmgen_default_byoc_c_main_2_consts.empty()) << "C source module hasn't been initialized.";

  byoc_c_bias_add(byoc_c_2_i0, (float*)(tvmgen_default_byoc_c_main_2_consts[0]->data), out0, 1, 1, 208, 208, 1, "bias_add");

}

int tvmgen_default_byoc_c_main_2_wrapper_(DLTensor* arg0,
	DLTensor* out0) {
  tvmgen_default_byoc_c_main_2_((float*)(arg0->data),
  (float*)(out0->data));
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif
TVM_DLL int32_t tvmgen_default_byoc_c_main_2(TVMValue* args, int* type_code, int num_args, TVMValue* out_value, int* out_type_code) {
  DLTensor* arg0 = (DLTensor*)(((TVMValue*)args)[0].v_handle);
  DLTensor* ret1 = (DLTensor*)(((TVMValue*)args)[1].v_handle);
  tvmgen_default_byoc_c_main_2_wrapper_(arg0,ret1);
  return 0;
}
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
int tvmgen_default_byoc_c_main_2_init_wrapper_(tvm::runtime::Array<tvm::runtime::NDArray> arr) {
  tvmgen_default_byoc_c_main_2_consts = arr;
return 0;
}

TVM_DLL_EXPORT_TYPED_FUNC(__init_tvmgen_default_byoc_c_main_2, tvmgen_default_byoc_c_main_2_init_wrapper_);

#endif
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <tvm/runtime/c_runtime_api.h>
#include <tvm/runtime/c_backend_api.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/packed_func.h>
#include <byoc_c/byoc_c_kernel.h>
using namespace tvm::runtime;
using namespace tvm::runtime::contrib;
#ifdef __cplusplus
tvm::runtime::Array<tvm::runtime::NDArray> tvmgen_default_byoc_c_main_0_consts;
#endif

void tvmgen_default_byoc_c_main_0_(float* byoc_c_0_i0, float* out0) {
  ICHECK(!tvmgen_default_byoc_c_main_0_consts.empty()) << "C source module hasn't been initialized.";

  byoc_c_conv2d(byoc_c_0_i0, (float*)(tvmgen_default_byoc_c_main_0_consts[0]->data), out0, 1, 3, 210, 210, 1, 3, 3, 1, 1, 0, 0, 0, 0, "conv2d");

}

int tvmgen_default_byoc_c_main_0_wrapper_(DLTensor* arg0,
	DLTensor* out0) {
  tvmgen_default_byoc_c_main_0_((float*)(arg0->data),
  (float*)(out0->data));
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif
TVM_DLL int32_t tvmgen_default_byoc_c_main_0(TVMValue* args, int* type_code, int num_args, TVMValue* out_value, int* out_type_code) {
  DLTensor* arg0 = (DLTensor*)(((TVMValue*)args)[0].v_handle);
  DLTensor* ret1 = (DLTensor*)(((TVMValue*)args)[1].v_handle);
  tvmgen_default_byoc_c_main_0_wrapper_(arg0,ret1);
  return 0;
}
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
int tvmgen_default_byoc_c_main_0_init_wrapper_(tvm::runtime::Array<tvm::runtime::NDArray> arr) {
  tvmgen_default_byoc_c_main_0_consts = arr;
return 0;
}

TVM_DLL_EXPORT_TYPED_FUNC(__init_tvmgen_default_byoc_c_main_0, tvmgen_default_byoc_c_main_0_init_wrapper_);

#endif
