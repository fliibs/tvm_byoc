#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <bitset>

class MemoryManager {
private:
    const int memorySize;
    std::vector<bool> memoryMap; 
    std::vector<std::pair<int, int>> allocationTable; 

public:
    MemoryManager(int size) : memorySize(size), memoryMap(size, false) {}

    void* myMalloc(int size) {
        int start = -1;
        int count = 0;
        for (int i = 0; i < memorySize; ++i) {
            if (!memoryMap[i]) {
                if (start == -1) {
                    start = i;
                }
                count++;
                if (count == size) {
                    break;
                }
            } else {
                start = -1;
                count = 0;
            }
        }

        if (count == size) {
            for (int i = start; i < start + size; ++i) {
                memoryMap[i] = true;
            }
            allocationTable.emplace_back(start, size);
            std::sort(allocationTable.begin(), allocationTable.end());
            return reinterpret_cast<void*>(start);
        }

        std::cout << "Could not allocate " << size << " Byte.\n";
        return nullptr;
    }

    void myFree(void* ptr) {
        int start = reinterpret_cast<intptr_t>(ptr);

        // auto removeIt = std::remove_if(allocationTable.begin(), allocationTable.end(),
                                //    [start](const auto& entry) { return entry.first == start; });
        auto removeIt = std::find_if(allocationTable.begin(), allocationTable.end(),
                                 [start](const auto& entry) { return entry.first == start; });
        
        if (removeIt != allocationTable.end()) {
            for (int i = start; i < start + removeIt->second; ++i) {
                memoryMap[i] = false;
            }
            allocationTable.erase(removeIt);
            // std::sort(allocationTable.begin(), allocationTable.end());
        } else {
            std::cout << "Error: can not free this memoru block. \n";
        }
    }

    void printMemoryStatus() const {
        std::cout << "\nMemory usage: \n";
        for (const auto& entry : allocationTable) {
            std::cout << "BaseAddress: " << entry.first << ", Size: " << entry.second << " Byte\n";
        }

        std::cout << "Memory free: \n";
        int startFree = -1;
        for (int i = 0; i < memorySize; ++i) {
            if (!memoryMap[i]) {
                if (startFree == -1) {
                    startFree = i;
                }
            } else {
                if (startFree != -1) {
                    std::cout << "BaseAddress: " << startFree << ", Size: " << i - startFree << " Byte\n";
                    startFree = -1;
                }
            }
        }

        if (startFree != -1) {
            std::cout << "BaseAddress: " << startFree << ", Size: " << memorySize - startFree << " Byte\n";
        }
    }
}; // MemoryManager


template <size_t NumRegisters>
class RegisterManager {
private:
    std::bitset<NumRegisters> registerStatus;

public:
    int allocateRegister() {
        for (size_t i = 0; i < NumRegisters; ++i) {
            if (!registerStatus[i]) {
                registerStatus.set(i);
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void deallocateRegister(int regIndex) {
        if (regIndex >= 0 && static_cast<size_t>(regIndex) < NumRegisters) {
            registerStatus.reset(static_cast<size_t>(regIndex));
        }
    }

    void clearAllRegisters() {
        registerStatus.reset();
    }

    void printRegisterStatus() {
        std::cout << "Register Status: " << registerStatus << std::endl;
    }
}; // RegisterManager

template<typename T>
void writeFloatToBinaryFile(const T& value, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary | std::ios::app);

    if (outfile.is_open()) {
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(float));
        outfile.close();
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
} // writeFloatToBinaryFile


template<typename T>
void writeValueToTxtFile(const T& value, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::app);

    if (outfile.is_open()) {
        outfile << value << std::endl;
        outfile.close();
    } else {
        std::cerr << "Unable to open the file: " << filename << std::endl;
    }
}

template<typename T>
void writeValueToHexFile(const T& value, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary | std::ios::app);

    if (!outfile.is_open()) {
        std::cerr << "Unable to open the file: " << filename << std::endl;
    } else {
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(T)); 
        outfile.close();
    }
}

void printResult(float* data, int p_N_, int p_C_, int p_H_, int p_W_, int layer_id__, std::string name, std::string path) {
    for (int n = 0; n < p_N_; ++n) {          
        for (int c = 0; c < p_C_; ++c) { 
            for (int w = 0; w < p_W_; ++w) {
                for (int h = 0; h < p_H_; ++h) {     
                    std::string outputString = "[" + std::to_string(n + 1) + ", " + std::to_string(c + 1) + ", "
                            + std::to_string(w + 1) + ", " + std::to_string(h + 1) + "] data is "
                            + std::to_string(data[n*p_C_*p_W_*p_H_+c*p_W_*p_H_+w*p_H_+h]);
                    writeValueToTxtFile(outputString, path+name+std::to_string(layer_id__)+"_input.txt");
                }
            }
        }                                         
    } 
}


void InputTransformToFile(float* input, int p_N_, int p_C_, int p_H_, int p_W_, int layer_id__, std::string name, std::string path){
    int pipe = 0;
    std::string file_name;
    std::string hex_file_name;
    std::string txt_file_name;
    for (int n = 0; n < p_N_; ++n) {
        for (int c = 0; c < p_C_; ++c){
            for (int h = 0; h < p_H_; ++h) {
                for (int w = 0; w < p_W_; ++w) {
                    pipe = static_cast<int>(4 * static_cast<double>(h) / p_H_);
                    file_name = path;
                    file_name += name+std::to_string(layer_id__);
                    file_name += "_batch" + std::to_string(n);
                    file_name += "_channel" + std::to_string(c);
                    file_name += "_pipe" + std::to_string(0);
                    hex_file_name = file_name + "_input.hex";
                    txt_file_name = file_name + "_input.txt";
                    writeValueToHexFile(input[n*p_C_*p_H_*p_W_+c*p_H_*p_W_+h*p_W_+w], hex_file_name);
                    writeValueToTxtFile(std::to_string(input[n*p_C_*p_H_*p_W_+c*p_H_*p_W_+h*p_W_+w]), txt_file_name);
                }
            }
        }
    }
}

