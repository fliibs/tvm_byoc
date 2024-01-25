
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

namespace tvm {
namespace runtime {
namespace contrib {

void byoc_c_conv2d(float* data, float* weights, float* out, int p_N_, int p_C_, int p_H_,
                            int p_W_, int p_O_, int p_Kh_, int p_Kw_, int p_Sh_, int p_Sw_, 
                            int p_Ph0_, int p_Ph1_, int p_Pw0_, int p_Pw1_) {
        
        for (int kh = 0; kh < p_Kh_; ++kh) {          
            for (int kw = 0; kw < p_Kw_; ++kw) {      
                printf("%f\n",weights[kh*p_Kw_+kw]);
                writeFloatToBinaryFile(weights[kh*p_Kw_+kw], "output.bin");
            }                                         
        } 

        for (int n = 0; n < p_N_; ++n) {
            for (int o = 0; o < p_O_; ++o){
                for (int h = 0; h < p_H_; ++h) {
                    for (int w = 0; w < p_W_; ++w) {
                        out[n*p_O_*p_H_*p_W_+o*p_H_*p_W_+h*p_W_+w] = 1;
                        
                    }
                }
            }
        }
        std::cout << "conv2d finished" << std::endl;
} // byoc_c_conv2d

void byoc_c_bias_add(float* data, float* bias, float* out, int p_N_, int p_C_, int p_H_, int p_W_, int axis) {
        for (int n = 0; n < p_N_; ++n) {          
            for (int c = 0; c < p_C_; ++c) { 
                for (int w = 0; w < p_W_; ++w) {
                    for (int h = 0; h < p_H_; ++h) {     
                        // printf("[%d, %d, %d, %d] relu data is %f\n",n+1, c+1, w+1, h+1  , data[n*p_C_*p_W_*p_H_+c*p_W_*p_H_+w*p_H_+h]);
                        std::string outputString = "[" + std::to_string(n + 1) + ", " + std::to_string(c + 1) + ", "
                              + std::to_string(w + 1) + ", " + std::to_string(h + 1) + "] bias_add data is "
                              + std::to_string(data[n*p_C_*p_W_*p_H_+c*p_W_*p_H_+w*p_H_+h]) + "\n";
                        writeStringToTxtFile(outputString, "bias_add_output.txt");
                        out[n*p_C_*p_H_*p_W_+c*p_H_*p_W_+h*p_W_+w] = 2;
                    }
                }
            }                                         
        } 
        std::cout << "bias_add finished" << std::endl;
} // byoc_c_bias_add

void byoc_c_relu(float* data, float* out, int p_N_, int p_C_, int p_H_, int p_W_) {
        for (int n = 0; n < p_N_; ++n) {          
            for (int c = 0; c < p_C_; ++c) { 
                for (int w = 0; w < p_W_; ++w) {
                    for (int h = 0; h < p_H_; ++h) {     
                        // printf("[%d, %d, %d, %d] relu data is %f\n",n+1, c+1, w+1, h+1  , data[n*p_C_*p_W_*p_H_+c*p_W_*p_H_+w*p_H_+h]);
                        std::string outputString = "[" + std::to_string(n + 1) + ", " + std::to_string(c + 1) + ", "
                              + std::to_string(w + 1) + ", " + std::to_string(h + 1) + "] relu data is "
                              + std::to_string(data[n*p_C_*p_W_*p_H_+c*p_W_*p_H_+w*p_H_+h]) + "\n";
                        writeStringToTxtFile(outputString, "relu_output.txt");
                    }
                }
            }                                         
        } 
        std::cout << "relu finished" << std::endl;
} // byoc_c_relu



void writeFloatToBinaryFile(float value, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary | std::ios::app);

    if (outfile.is_open()) {
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(float));
        outfile.close();
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
} // writeFloatToBinaryFile

void writeStringToTxtFile(std::string content, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::app);

    if (outfile.is_open()) {
        outfile << content;
        outfile.close();
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
} // writeStringToTxtFile


}// contrib
}// runtime
}// tvm