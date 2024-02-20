#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

#include <cmath>


#include "beyond_utils.h"
#include "beyond_kernel.h"
// #define TARGET_MODE
#include "asm.h"

namespace tvm {
namespace runtime {
namespace contrib {

// Register
const size_t VRSize = 1024;
const size_t numberOfVectorRegisters = 32;
const size_t numberOfScalarRegisters = 32;
const size_t ElementWidth = VRSize/numberOfVectorRegisters;

RegisterManager<numberOfVectorRegisters> BeyondVecRegManager;
RegisterManager<numberOfScalarRegisters> BeyondSclRegManager;


void beyond_relu(float* data, float* out, int p_N_, int p_C_, int p_H_, int p_W_, std::string name) {
    for (int n = 0; n < p_N_; ++n) {
        float* rs_0 = reinterpret_cast<float*>(BeyondSclRegManager.allocateRegister());
        float* rs_1 = reinterpret_cast<float*>(BeyondSclRegManager.allocateRegister());
        float* rs_2 = reinterpret_cast<float*>(BeyondSclRegManager.allocateRegister());
        float* vd_0 = reinterpret_cast<float*>(BeyondVecRegManager.allocateRegister());
        float* vd_1 = reinterpret_cast<float*>(BeyondVecRegManager.allocateRegister());
        float* vd_2 = reinterpret_cast<float*>(BeyondVecRegManager.allocateRegister());
        float const_zero = 0;
        LOAD(rs_1, &const_zero, 0);
        // std::cout << data << "," << out << "\n";
        for (int i = 0; i < static_cast<int>(std::ceil(p_H_*p_W_*p_C_/ElementWidth)); ++i) {
            // riscv vector load
            float* vec_load_address = data + i;
            // int vec_load_address = reinterpret_cast<std::intptr_t>(data) + i*4;
            LOAD(rs_0, vec_load_address, 0);
            RVV_VLOAD(vd_0, rs_0);
            // riscv vector min
            RVV_VMIN(vd_1, vd_0, rs_1);
            // riscv vector store
            float* vec_store_address = out + i;
            // int vec_store_address = reinterpret_cast<std::intptr_t>(out) + i*4;
            LOAD(rs_2, vec_store_address, 0);
            RVV_VSTORE(vd_2, rs_2);
            // BydwriteValueToTxtFile(vec_load_address, "param/address.txt");
        }
    }


} // byoc_func_relu

}// contrib
}// runtime
}// tvm