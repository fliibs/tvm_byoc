#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <fstream>

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

    // void compressMemory() {
    //     for (int i = 0; i < allocationTable.size() - 1; ++i) {
    //         int endCurr = allocationTable[i].first + allocationTable[i].second;
    //         int startNext = allocationTable[i + 1].first;

    //         if (endCurr < startNext) {
    //             for (int j = endCurr; j < startNext; ++j) {
    //                 memoryMap[j] = false;
    //             }

    //             allocationTable[i + 1].first = allocationTable[i].first + allocationTable[i].second;
    //         }
    //     }
    // }


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
};

// int main() {
//     MemoryManager memoryManager(10 * 1024 * 1024); 

//     void* block1 = memoryManager.myMalloc(1024);
//     void* block2 = memoryManager.myMalloc(2048);
//     void* block4 = memoryManager.myMalloc(3072);

//     memoryManager.printMemoryStatus();

//     memoryManager.myFree(block2);

//     memoryManager.printMemoryStatus();

//     void* block3 = memoryManager.myMalloc(4096);
//     void* block5 = memoryManager.myMalloc(1024);

//     memoryManager.printMemoryStatus();

//     return 0;


void writeFloatToBinaryFile(float value, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary | std::ios::app);

    if (outfile.is_open()) {
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(float));
        outfile.close();
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
} // writeFloatToBinaryFile

void writeFloatToHexFile(float value, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary | std::ios::app);

    if (!outfile.is_open()) {
        std::cerr << "Unable to open the file." << filename << std::endl;
    } else {
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(float)); 
        outfile.close();
    }
} // writeFloatToHexFile

void writeStringToTxtFile(std::string content, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::app);

    if (outfile.is_open()) {
        outfile << content << std::endl;
        outfile.close();
    } else {
        std::cerr << "Unable to open the file." << std::endl;
    }
} // writeStringToTxtFile

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
                    file_name += "_pipe" + std::to_string(pipe);
                    hex_file_name = file_name + "_input.hex";
                    txt_file_name = file_name + "_input.txt";
                    writeFloatToHexFile(input[n*p_C_*p_H_*p_W_+c*p_H_*p_W_+h*p_W_+w], hex_file_name);
                    writeStringToTxtFile(std::to_string(input[n*p_C_*p_H_*p_W_+c*p_H_*p_W_+h*p_W_+w]), txt_file_name);
                }
            }
        }
    }
}
