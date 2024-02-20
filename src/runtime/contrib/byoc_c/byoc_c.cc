
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

#include <iostream>
#include <fstream>

#include <cmath>

// #include <bitset>

#include "byoc_c_kernel.h"
#include "byoc_utils.h"
#include "instruction.h"

namespace tvm {
namespace runtime {
namespace contrib {

static int layer_id__=0;
// Memory
MemoryManager memoryManager(10 * 1024 * 1024);
static void* output_ptr = static_cast<void*>(static_cast<intptr_t>(0));
const uint32_t npu_base_address  = 1024*1024*1024; 
// Register
const size_t VRSize = 1024;
const size_t numberOfVectorRegisters = 32;
const size_t numberOfScalarRegisters = 32;
const size_t ElementWidth = VRSize/numberOfVectorRegisters;

RegisterManager<numberOfVectorRegisters> VectorRegisterManager;
RegisterManager<numberOfScalarRegisters> ScalarRegisterManager;


void byoc_c_conv2d(float* data, float* weights, float* out, int p_N_, int p_C_, int p_H_,
                            int p_W_, int p_O_, int p_Kh_, int p_Kw_, int p_Sh_, int p_Sw_, 
                            int p_Ph0_, int p_Ph1_, int p_Pw0_, int p_Pw1_, std::string name) {
        
        int OH_ = p_H_-p_Kh_+p_Ph0_+p_Ph1_+1;
        int OW_ = p_W_-p_Kw_+p_Pw0_+p_Pw1_+1;

        void* input_block;
        if (layer_id__==0) {
            input_block = memoryManager.myMalloc(p_N_*p_C_*p_H_*p_W_*4);
        } else {
            input_block = output_ptr;
        }
        void* weight_block     = memoryManager.myMalloc(p_O_*p_C_*p_Kh_*p_Kw_*4);
        void* output_block     = memoryManager.myMalloc(p_N_*p_O_*OH_*OW_*4);
        // int input_base_address = reinterpret_cast<intptr_t>(input_block);
        // int weight_base_addrss = reinterpret_cast<intptr_t>(weight_block);
        // int output_base_addrss = reinterpret_cast<intptr_t>(output_block);
        
        std::cout << "\n========conv2d " << std::to_string(layer_id__) << " start========" << std::endl;
        memoryManager.printMemoryStatus();
        
        // gen kernel file
        for (int o = 0; o < p_O_; ++o){
            for (int c = 0; c < p_C_; ++c){
                for (int kh = 0; kh < p_Kh_; ++kh) {          
                    for (int kw = 0; kw < p_Kw_; ++kw) {      
                        writeValueToHexFile(weights[o*p_C_*p_Kh_*p_Kw_+c*p_Kh_*p_Kw_+kh*p_Kw_+kw], "param/"+name+std::to_string(layer_id__)+"_kernel.hex");
                    }                                         
                } 
            }
        }
        writeValueToHexFile(0, "param/"+name+std::to_string(layer_id__)+"_kernel.hex");

        // gen input file 
        if (layer_id__==0){
            InputTransformToFile(data, p_N_, p_C_, p_H_, p_W_, layer_id__, name, "param/");
        }

        memoryManager.myFree(input_block);
        memoryManager.myFree(weight_block);
        memoryManager.printMemoryStatus();
        output_ptr    = output_block;
        layer_id__++;
        std::cout << "conv2d finished" << std::endl;
} // byoc_c_conv2d

void byoc_c_bias_add(float* data, float* bias, float* out, int p_N_, int p_C_, int p_H_, int p_W_, int axis, std::string name) {
        void* input_block = output_ptr;
        void* weight_block = memoryManager.myMalloc(p_N_*p_C_*4);
        void* output_block = memoryManager.myMalloc(p_N_*p_C_*p_H_*p_W_*4);
        
        std::cout << "\n=======bias_add " << std::to_string(layer_id__) << " start=========" << std::endl;
        memoryManager.printMemoryStatus();

        // print result
        printResult(data, p_N_, p_C_, p_H_, p_W_, layer_id__, name, "param/");

        for (int n = 0; n < p_N_; ++n) {          
            for (int c = 0; c < p_C_; ++c) { 
                writeValueToHexFile(bias[n*p_C_+c], "param/"+name+std::to_string(layer_id__)+"_kernel.hex");
            }                                         
        } 
        memoryManager.myFree(input_block);
        memoryManager.myFree(weight_block);
        memoryManager.printMemoryStatus();
        output_ptr    = output_block;
        layer_id__++;
        std::cout << "bias_add finished" << std::endl;
} // byoc_c_bias_add

void byoc_c_relu(float* data, float* out, int p_N_, int p_C_, int p_H_, int p_W_, std::string name) {
        void* input_block = output_ptr;
        void* output_block = memoryManager.myMalloc(p_N_*p_C_*p_H_*p_W_*4);
        
        int input_base_address = reinterpret_cast<intptr_t>(input_block);
        int output_base_addrss = reinterpret_cast<intptr_t>(output_block);
        
        std::cout << "\n========relu " << std::to_string(layer_id__) << " start=========" << std::endl;
        memoryManager.printMemoryStatus();

        // print result
        printResult(data, p_N_, p_C_, p_H_, p_W_, layer_id__, name, "param/");
        // gen input file 
        if (layer_id__==0){
            InputTransformToFile(data, p_N_, p_C_, p_H_, p_W_, layer_id__, name, "param/");
        }

        // gen instruction
        std::bitset<32> inst_relu;
        uint32_t vec_load_address;
        uint32_t vec_store_address;
        uint32_t vec_load_inst;
        uint32_t vec_arth_inst;
        uint32_t vec_store_inst;

        for (int n = 0; n < p_N_; ++n) { // always 1
            int rs_zero = ScalarRegisterManager.allocateRegister();     // register contain constant 0
            int rs1     = ScalarRegisterManager.allocateRegister();     // register contain input_base_address
            int rs2     = ScalarRegisterManager.allocateRegister();     // register contain output_base_address
            int vd1     = VectorRegisterManager.allocateRegister();     // load 
            int vd2     = VectorRegisterManager.allocateRegister();     // arth 
            int vd3     = VectorRegisterManager.allocateRegister();     // store
            for (int i = 0; i < static_cast<int>(std::ceil(p_H_*p_W_*p_C_/ElementWidth)); ++i) {
                vec_load_address    = npu_base_address + input_base_address + i*4;
                vec_store_address   = npu_base_address + output_base_addrss + i*4;
                writeValueToHexFile(vec_load_address,"instruction/base_address.hex");
                writeValueToHexFile(rs_zero,"instruction/base_address.hex");
                writeValueToHexFile(vec_store_address,"instruction/base_address.hex");
                // Load data 
                uint8_t vd_load = static_cast<uint8_t>(vd1);
                uint8_t rs1_load = static_cast<uint8_t>(rs1);
                vec_load_inst = static_cast<uint32_t>(RVVLoadInst(vd_load, rs1_load));
                writeValueToHexFile(vec_load_inst,"instruction/instruction.hex");
                // Min/Max
                uint8_t funct6_max = 0b000100;
                uint8_t vs2_max    = static_cast<uint8_t>(vd1);
                uint8_t rs1_max    = static_cast<uint8_t>(rs_zero);
                uint8_t vd_max    = static_cast<uint8_t>(vd2);
                uint8_t funct3_max = 0b100;
                vec_arth_inst = static_cast<uint32_t>(RVVArthInst(funct6_max, vs2_max, rs1_max, vd_max, funct3_max));
                writeValueToHexFile(vec_arth_inst,"instruction/instruction.hex");
                // Store data
                uint8_t vd_store = static_cast<uint8_t>(vd3);
                uint8_t rs1_store = static_cast<uint8_t>(rs2);
                vec_store_inst = static_cast<uint32_t>(RVVStoreInst(vd_store, rs1_store));
                writeValueToHexFile(vec_store_inst,"instruction/instruction.hex");
            }
            VectorRegisterManager.clearAllRegisters();
            ScalarRegisterManager.clearAllRegisters();
        }

        memoryManager.myFree(input_block);
        memoryManager.printMemoryStatus();
        output_ptr    = output_block;
        layer_id__++;
        std::cout << "relu finished" << std::endl;
} // byoc_c_relu



}// contrib
}// runtime
}// tvm