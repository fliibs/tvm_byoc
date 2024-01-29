
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <iostream>
#include <fstream>

#include "byoc_c_kernel.h"
#include "byoc_utils.h"

namespace tvm {
namespace runtime {
namespace contrib {

static int layer_id__=0;
MemoryManager memoryManager(10 * 1024 * 1024);
static void* output_ptr = static_cast<void*>(static_cast<intptr_t>(0));

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
        int input_base_address = reinterpret_cast<intptr_t>(input_block);
        void* weight_block     = memoryManager.myMalloc(p_O_*p_C_*p_Kh_*p_Kw_*4);
        void* output_block     = memoryManager.myMalloc(p_N_*p_O_*OH_*OW_*4);
        int weight_base_addrss = reinterpret_cast<intptr_t>(weight_block);
        int output_base_addrss = reinterpret_cast<intptr_t>(output_block);
        
        std::cout << "\n========conv2d " << std::to_string(layer_id__) << " start========" << std::endl;
        memoryManager.printMemoryStatus();
        
        // gen kernel file
        for (int o = 0; o < p_O_; ++o){
            for (int c = 0; c < p_C_; ++c){
                for (int kh = 0; kh < p_Kh_; ++kh) {          
                    for (int kw = 0; kw < p_Kw_; ++kw) {      
                        writeFloatToHexFile(weights[o*p_C_*p_Kh_*p_Kw_+c*p_Kh_*p_Kw_+kh*p_Kw_+kw], "param/"+name+std::to_string(layer_id__)+"_kernel.hex");
                    }                                         
                } 
            }
        }
        writeFloatToHexFile(0, "param/"+name+std::to_string(layer_id__)+"_kernel.hex");

        // gen input file 
        if (layer_id__==0){
            InputTransformToFile(data, p_N_, p_C_, p_H_, p_W_, layer_id__, name, "param/");
        }

        // for (int n = 0; n < p_N_; ++n) {
        //     for (int o = 0; o < p_O_; ++o){
        //         for (int h = 0; h < OH_; ++h) {
        //             for (int w = 0; w < OW_; ++w) {
        //                 out[n*p_O_*OH_*OW_+o*OH_*OW_+h*OW_+w] = n*p_O_*OH_*OW_+o*OH_*OW_+h*OW_+w;
        //             }
        //         }
        //     }
        // }

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

        for (int n = 0; n < p_N_; ++n) {          
            for (int c = 0; c < p_C_; ++c) { 
                writeFloatToHexFile(bias[n*p_C_+c], "param/"+name+std::to_string(layer_id__)+"_kernel.hex");
                for (int h = 0; h < p_H_; ++h) {
                    for (int w = 0; w < p_W_; ++w) {     
                        std::string outputString = "[" + std::to_string(n + 1) + ", " + std::to_string(c + 1) + ", "
                              + std::to_string(w + 1) + ", " + std::to_string(h + 1) + "] bias_add data is "
                              + std::to_string(data[n*p_C_*p_W_*p_H_+c*p_W_*p_H_+h*p_W_+w]) + "\n";
                        writeStringToTxtFile(outputString, "param/"+name+std::to_string(layer_id__)+"_output.txt");
                        out[n*p_C_*p_W_*p_H_+c*p_W_*p_H_+h*p_W_+w] = 2;
                    }
                }
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
        
        std::cout << "\n========relu " << std::to_string(layer_id__) << " start=========" << std::endl;
        memoryManager.printMemoryStatus();

        for (int n = 0; n < p_N_; ++n) {          
            for (int c = 0; c < p_C_; ++c) { 
                for (int w = 0; w < p_W_; ++w) {
                    for (int h = 0; h < p_H_; ++h) {     
                        // printf("[%d, %d, %d, %d] relu data is %f\n",n+1, c+1, w+1, h+1  , data[n*p_C_*p_W_*p_H_+c*p_W_*p_H_+w*p_H_+h]);
                        std::string outputString = "[" + std::to_string(n + 1) + ", " + std::to_string(c + 1) + ", "
                              + std::to_string(w + 1) + ", " + std::to_string(h + 1) + "] relu data is "
                              + std::to_string(data[n*p_C_*p_W_*p_H_+c*p_W_*p_H_+w*p_H_+h]) + "\n";
                        writeStringToTxtFile(outputString, "param/"+name+std::to_string(layer_id__)+"_output.txt");
                    }
                }
            }                                         
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