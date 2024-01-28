// tvm target: c -keys=cpu 
#define TVM_EXPORTS
#include "tvm/runtime/c_runtime_api.h"
#include "tvm/runtime/c_backend_api.h"
#include <math.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
#endif
TVM_DLL int32_t tvmgen_default_fused_layout_transform(void* args, int32_t* arg_type_ids, int32_t num_args, void* out_ret_value, int32_t* out_ret_tcode, void* resource_handle);
#ifdef __cplusplus
extern "C"
#endif
TVM_DLL int32_t tvmgen_default_fused_layout_transform_1(void* args, int32_t* arg_type_ids, int32_t num_args, void* out_ret_value, int32_t* out_ret_tcode, void* resource_handle);
#ifdef __cplusplus
extern "C"
#endif
TVM_DLL int32_t tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add(void* args, int32_t* arg_type_ids, int32_t num_args, void* out_ret_value, int32_t* out_ret_tcode, void* resource_handle);
#ifdef __cplusplus
extern "C"
#endif
TVM_DLL int32_t tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu(void* args, int32_t* arg_type_ids, int32_t num_args, void* out_ret_value, int32_t* out_ret_tcode, void* resource_handle);
#ifdef __cplusplus
extern "C"
#endif
TVM_DLL int32_t tvmgen_default_fused_layout_transform(void* args, int32_t* arg_type_ids, int32_t num_args, void* out_ret_value, int32_t* out_ret_tcode, void* resource_handle) {
  int32_t p0_code = arg_type_ids[0];
  int32_t T_layout_trans_code = arg_type_ids[1];
  void* p0 = (((TVMValue*)args)[0].v_handle);
  void* T_layout_trans = (((TVMValue*)args)[1].v_handle);
  void* p0_1 = (((DLTensor*)p0)[0].data);
  void* tvmgen_default_fused_layout_transform_p0_shape = (((DLTensor*)p0)[0].shape);
  void* tvmgen_default_fused_layout_transform_p0_strides = (((DLTensor*)p0)[0].strides);
  int32_t dev_id = (((DLTensor*)p0)[0].device.device_id);
  void* T_layout_trans_1 = (((DLTensor*)T_layout_trans)[0].data);
  void* tvmgen_default_fused_layout_transform_T_layout_trans_shape = (((DLTensor*)T_layout_trans)[0].shape);
  void* tvmgen_default_fused_layout_transform_T_layout_trans_strides = (((DLTensor*)T_layout_trans)[0].strides);
  if (!(tvmgen_default_fused_layout_transform_p0_strides == NULL)) {
  }
  if (!(tvmgen_default_fused_layout_transform_T_layout_trans_strides == NULL)) {
  }
  for (int32_t ax0_ax1_fused_ax2_fused = 0; ax0_ax1_fused_ax2_fused < 630; ++ax0_ax1_fused_ax2_fused) {
    for (int32_t ax3 = 0; ax3 < 210; ++ax3) {
      int32_t cse_var_1 = ((ax0_ax1_fused_ax2_fused * 210) + ax3);
      ((float*)T_layout_trans_1)[cse_var_1] = ((float*)p0_1)[cse_var_1];
    }
  }
  return 0;
}

#ifdef __cplusplus
extern "C"
#endif
TVM_DLL int32_t tvmgen_default_fused_layout_transform_1(void* args, int32_t* arg_type_ids, int32_t num_args, void* out_ret_value, int32_t* out_ret_tcode, void* resource_handle) {
  int32_t p0_code = arg_type_ids[0];
  int32_t T_layout_trans_code = arg_type_ids[1];
  void* p0 = (((TVMValue*)args)[0].v_handle);
  void* T_layout_trans = (((TVMValue*)args)[1].v_handle);
  void* p0_1 = (((DLTensor*)p0)[0].data);
  void* tvmgen_default_fused_layout_transform_1_p0_shape = (((DLTensor*)p0)[0].shape);
  void* tvmgen_default_fused_layout_transform_1_p0_strides = (((DLTensor*)p0)[0].strides);
  int32_t dev_id = (((DLTensor*)p0)[0].device.device_id);
  void* T_layout_trans_1 = (((DLTensor*)T_layout_trans)[0].data);
  void* tvmgen_default_fused_layout_transform_1_T_layout_trans_shape = (((DLTensor*)T_layout_trans)[0].shape);
  void* tvmgen_default_fused_layout_transform_1_T_layout_trans_strides = (((DLTensor*)T_layout_trans)[0].strides);
  if (!(tvmgen_default_fused_layout_transform_1_p0_strides == NULL)) {
  }
  if (!(tvmgen_default_fused_layout_transform_1_T_layout_trans_strides == NULL)) {
  }
  for (int32_t ax0_ax1_fused = 0; ax0_ax1_fused < 3; ++ax0_ax1_fused) {
    for (int32_t ax2 = 0; ax2 < 206; ++ax2) {
      for (int32_t ax3_outer = 0; ax3_outer < 13; ++ax3_outer) {
        for (int32_t ax3_inner_s = 0; ax3_inner_s < 16; ++ax3_inner_s) {
          if (((ax3_outer * 8) + (ax3_inner_s >> 1)) < 103) {
            ((float*)T_layout_trans_1)[((((ax0_ax1_fused * 42436) + (ax2 * 206)) + (ax3_outer * 16)) + ax3_inner_s)] = ((float*)p0_1)[((((ax2 * 618) + (ax3_outer * 48)) + (ax3_inner_s * 3)) + ax0_ax1_fused)];
          }
        }
      }
    }
  }
  return 0;
}

#ifdef __cplusplus
extern "C"
#endif
TVM_DLL int32_t tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add(void* args, int32_t* arg_type_ids, int32_t num_args, void* out_ret_value, int32_t* out_ret_tcode, void* resource_handle) {
  int32_t p0_code = arg_type_ids[0];
  int32_t p1_code = arg_type_ids[1];
  int32_t p2_code = arg_type_ids[2];
  int32_t T_add_code = arg_type_ids[3];
  void* p0 = (((TVMValue*)args)[0].v_handle);
  void* p1 = (((TVMValue*)args)[1].v_handle);
  void* p2 = (((TVMValue*)args)[2].v_handle);
  void* T_add = (((TVMValue*)args)[3].v_handle);
  void* p0_1 = (((DLTensor*)p0)[0].data);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_p0_shape = (((DLTensor*)p0)[0].shape);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_p0_strides = (((DLTensor*)p0)[0].strides);
  int32_t dev_id = (((DLTensor*)p0)[0].device.device_id);
  void* p1_1 = (((DLTensor*)p1)[0].data);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_p1_shape = (((DLTensor*)p1)[0].shape);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_p1_strides = (((DLTensor*)p1)[0].strides);
  void* p2_1 = (((DLTensor*)p2)[0].data);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_p2_shape = (((DLTensor*)p2)[0].shape);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_p2_strides = (((DLTensor*)p2)[0].strides);
  void* T_add_1 = (((DLTensor*)T_add)[0].data);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_T_add_shape = (((DLTensor*)T_add)[0].shape);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_T_add_strides = (((DLTensor*)T_add)[0].strides);
  if (!(tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_p0_strides == NULL)) {
  }
  if (!(tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_p1_strides == NULL)) {
  }
  if (!(tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_p2_strides == NULL)) {
  }
  if (!(tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_T_add_strides == NULL)) {
  }
  for (int32_t ax0_ax1_fused_ax2_fused = 0; ax0_ax1_fused_ax2_fused < 206; ++ax0_ax1_fused_ax2_fused) {
    void* conv2d_NCHWc = TVMBackendAllocWorkspace(1, dev_id, (uint64_t)2472, 2, 32);
    if (conv2d_NCHWc == NULL) {
      return -1;
    }
    float3 conv2d_NCHWc_global[2];
    for (int32_t ow_outer = 0; ow_outer < 103; ++ow_outer) {
      conv2d_NCHWc_global[0] = ((float3)(0.000000e+00f, 0.000000e+00f, 0.000000e+00f));
      conv2d_NCHWc_global[1] = ((float3)(0.000000e+00f, 0.000000e+00f, 0.000000e+00f));
      for (int32_t kh = 0; kh < 3; ++kh) {
        for (int32_t kw = 0; kw < 3; ++kw) {
          int32_t cse_var_2 = ((kh * 9) + (kw * 3));
          int32_t cse_var_1 = ((((kh * 208) + (ax0_ax1_fused_ax2_fused * 208)) + (ow_outer * 2)) + kw);
          int32_t3 v_ = int32_t3((cse_var_2)+(1*0), (cse_var_2)+(1*1), (cse_var_2)+(1*2));
          conv2d_NCHWc_global[0] = (conv2d_NCHWc_global[0] + (((float3)(((float*)p0_1)[cse_var_1], ((float*)p0_1)[cse_var_1], ((float*)p0_1)[cse_var_1])) * (float3(((float*)p1_1)[v_.s0],((float*)p1_1)[v_.s1],((float*)p1_1)[v_.s2]))));
          conv2d_NCHWc_global[1] = (conv2d_NCHWc_global[1] + (((float3)(((float*)p0_1)[(cse_var_1 + 1)], ((float*)p0_1)[(cse_var_1 + 1)], ((float*)p0_1)[(cse_var_1 + 1)])) * (float3(((float*)p1_1)[v_.s0],((float*)p1_1)[v_.s1],((float*)p1_1)[v_.s2]))));
        }
      }
      for (int32_t ow_inner = 0; ow_inner < 2; ++ow_inner) {
        ((float3*)conv2d_NCHWc)[((ow_outer * 2) + ow_inner)] = conv2d_NCHWc_global[ow_inner];
      }
    }
    for (int32_t ax3_outer = 0; ax3_outer < 103; ++ax3_outer) {
      for (int32_t ax3_inner = 0; ax3_inner < 2; ++ax3_inner) {
        *(float3*)(((float*)T_add_1) + (((ax0_ax1_fused_ax2_fused * 618) + (ax3_outer * 6)) + (ax3_inner * 3))) = (((float3*)conv2d_NCHWc)[((ax3_outer * 2) + ax3_inner)] + *(float3*)(((float*)p2_1) + 0));
      }
    }
    if (TVMBackendFreeWorkspace(1, dev_id, conv2d_NCHWc) != 0) {
      return -1;
    }
  }
  return 0;
}

#ifdef __cplusplus
extern "C"
#endif
TVM_DLL int32_t tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu(void* args, int32_t* arg_type_ids, int32_t num_args, void* out_ret_value, int32_t* out_ret_tcode, void* resource_handle) {
  int32_t p0_code = arg_type_ids[0];
  int32_t p1_code = arg_type_ids[1];
  int32_t p2_code = arg_type_ids[2];
  int32_t T_relu_code = arg_type_ids[3];
  void* p0 = (((TVMValue*)args)[0].v_handle);
  void* p1 = (((TVMValue*)args)[1].v_handle);
  void* p2 = (((TVMValue*)args)[2].v_handle);
  void* T_relu = (((TVMValue*)args)[3].v_handle);
  void* p0_1 = (((DLTensor*)p0)[0].data);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_p0_shape = (((DLTensor*)p0)[0].shape);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_p0_strides = (((DLTensor*)p0)[0].strides);
  int32_t dev_id = (((DLTensor*)p0)[0].device.device_id);
  void* p1_1 = (((DLTensor*)p1)[0].data);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_p1_shape = (((DLTensor*)p1)[0].shape);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_p1_strides = (((DLTensor*)p1)[0].strides);
  void* p2_1 = (((DLTensor*)p2)[0].data);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_p2_shape = (((DLTensor*)p2)[0].shape);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_p2_strides = (((DLTensor*)p2)[0].strides);
  void* T_relu_1 = (((DLTensor*)T_relu)[0].data);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_T_relu_shape = (((DLTensor*)T_relu)[0].shape);
  void* tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_T_relu_strides = (((DLTensor*)T_relu)[0].strides);
  if (!(tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_p0_strides == NULL)) {
  }
  if (!(tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_p1_strides == NULL)) {
  }
  if (!(tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_p2_strides == NULL)) {
  }
  if (!(tvmgen_default_fused_nn_contrib_conv2d_NCHWc_add_nn_relu_T_relu_strides == NULL)) {
  }
  for (int32_t ax0_ax1_fused_ax2_fused = 0; ax0_ax1_fused_ax2_fused < 208; ++ax0_ax1_fused_ax2_fused) {
    float conv2d_NCHWc[208];
    float conv2d_NCHWc_global[26];
    for (int32_t ow_outer = 0; ow_outer < 8; ++ow_outer) {
      conv2d_NCHWc_global[0] = 0.000000e+00f;
      conv2d_NCHWc_global[1] = 0.000000e+00f;
      conv2d_NCHWc_global[2] = 0.000000e+00f;
      conv2d_NCHWc_global[3] = 0.000000e+00f;
      conv2d_NCHWc_global[4] = 0.000000e+00f;
      conv2d_NCHWc_global[5] = 0.000000e+00f;
      conv2d_NCHWc_global[6] = 0.000000e+00f;
      conv2d_NCHWc_global[7] = 0.000000e+00f;
      conv2d_NCHWc_global[8] = 0.000000e+00f;
      conv2d_NCHWc_global[9] = 0.000000e+00f;
      conv2d_NCHWc_global[10] = 0.000000e+00f;
      conv2d_NCHWc_global[11] = 0.000000e+00f;
      conv2d_NCHWc_global[12] = 0.000000e+00f;
      conv2d_NCHWc_global[13] = 0.000000e+00f;
      conv2d_NCHWc_global[14] = 0.000000e+00f;
      conv2d_NCHWc_global[15] = 0.000000e+00f;
      conv2d_NCHWc_global[16] = 0.000000e+00f;
      conv2d_NCHWc_global[17] = 0.000000e+00f;
      conv2d_NCHWc_global[18] = 0.000000e+00f;
      conv2d_NCHWc_global[19] = 0.000000e+00f;
      conv2d_NCHWc_global[20] = 0.000000e+00f;
      conv2d_NCHWc_global[21] = 0.000000e+00f;
      conv2d_NCHWc_global[22] = 0.000000e+00f;
      conv2d_NCHWc_global[23] = 0.000000e+00f;
      conv2d_NCHWc_global[24] = 0.000000e+00f;
      conv2d_NCHWc_global[25] = 0.000000e+00f;
      for (int32_t ic_outer = 0; ic_outer < 3; ++ic_outer) {
        for (int32_t kh = 0; kh < 3; ++kh) {
          for (int32_t kw = 0; kw < 3; ++kw) {
            int32_t cse_var_2 = (((ic_outer * 9) + (kh * 3)) + kw);
            int32_t cse_var_1 = (((((ic_outer * 44100) + (kh * 210)) + (ax0_ax1_fused_ax2_fused * 210)) + (ow_outer * 26)) + kw);
            conv2d_NCHWc_global[0] = (conv2d_NCHWc_global[0] + (((float*)p0_1)[cse_var_1] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[1] = (conv2d_NCHWc_global[1] + (((float*)p0_1)[(cse_var_1 + 1)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[2] = (conv2d_NCHWc_global[2] + (((float*)p0_1)[(cse_var_1 + 2)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[3] = (conv2d_NCHWc_global[3] + (((float*)p0_1)[(cse_var_1 + 3)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[4] = (conv2d_NCHWc_global[4] + (((float*)p0_1)[(cse_var_1 + 4)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[5] = (conv2d_NCHWc_global[5] + (((float*)p0_1)[(cse_var_1 + 5)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[6] = (conv2d_NCHWc_global[6] + (((float*)p0_1)[(cse_var_1 + 6)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[7] = (conv2d_NCHWc_global[7] + (((float*)p0_1)[(cse_var_1 + 7)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[8] = (conv2d_NCHWc_global[8] + (((float*)p0_1)[(cse_var_1 + 8)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[9] = (conv2d_NCHWc_global[9] + (((float*)p0_1)[(cse_var_1 + 9)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[10] = (conv2d_NCHWc_global[10] + (((float*)p0_1)[(cse_var_1 + 10)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[11] = (conv2d_NCHWc_global[11] + (((float*)p0_1)[(cse_var_1 + 11)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[12] = (conv2d_NCHWc_global[12] + (((float*)p0_1)[(cse_var_1 + 12)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[13] = (conv2d_NCHWc_global[13] + (((float*)p0_1)[(cse_var_1 + 13)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[14] = (conv2d_NCHWc_global[14] + (((float*)p0_1)[(cse_var_1 + 14)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[15] = (conv2d_NCHWc_global[15] + (((float*)p0_1)[(cse_var_1 + 15)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[16] = (conv2d_NCHWc_global[16] + (((float*)p0_1)[(cse_var_1 + 16)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[17] = (conv2d_NCHWc_global[17] + (((float*)p0_1)[(cse_var_1 + 17)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[18] = (conv2d_NCHWc_global[18] + (((float*)p0_1)[(cse_var_1 + 18)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[19] = (conv2d_NCHWc_global[19] + (((float*)p0_1)[(cse_var_1 + 19)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[20] = (conv2d_NCHWc_global[20] + (((float*)p0_1)[(cse_var_1 + 20)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[21] = (conv2d_NCHWc_global[21] + (((float*)p0_1)[(cse_var_1 + 21)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[22] = (conv2d_NCHWc_global[22] + (((float*)p0_1)[(cse_var_1 + 22)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[23] = (conv2d_NCHWc_global[23] + (((float*)p0_1)[(cse_var_1 + 23)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[24] = (conv2d_NCHWc_global[24] + (((float*)p0_1)[(cse_var_1 + 24)] * ((float*)p1_1)[cse_var_2]));
            conv2d_NCHWc_global[25] = (conv2d_NCHWc_global[25] + (((float*)p0_1)[(cse_var_1 + 25)] * ((float*)p1_1)[cse_var_2]));
          }
        }
      }
      for (int32_t ow_inner = 0; ow_inner < 26; ++ow_inner) {
        conv2d_NCHWc[((ow_outer * 26) + ow_inner)] = conv2d_NCHWc_global[ow_inner];
      }
    }
    for (int32_t ax3_outer = 0; ax3_outer < 8; ++ax3_outer) {
      for (int32_t ax3_inner = 0; ax3_inner < 26; ++ax3_inner) {
        int32_t cse_var_3 = (ax3_outer * 26);
        float v_ = conv2d_NCHWc[(cse_var_3 + ax3_inner)] + ((float*)p2_1)[0];
        ((float*)T_relu_1)[(((ax0_ax1_fused_ax2_fused * 208) + cse_var_3) + ax3_inner)] = ((v_) > (0.000000e+00f) ? (v_) : (0.000000e+00f));
      }
    }
  }
  return 0;
}

