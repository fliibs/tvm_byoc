#ifndef MATCH_OP_H_
#define MATCH_OP_H_

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
    // std::vector<std::pair<int, int>> allocationTable; 
    std::vector<std::tuple<int, int, std::string>> allocationTable;

public:
    MemoryManager(int size) : memorySize(size), memoryMap(size, false) {}

    void* myMalloc(const std::string& name, int size) {
        auto it = std::find_if(allocationTable.begin(), allocationTable.end(),
                               [name](const auto& entry) { return std::get<2>(entry) == name; });

        if (it != allocationTable.end()) {
            return reinterpret_cast<void*>(std::get<0>(*it));
        }

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
            allocationTable.emplace_back(start, size, name);
            std::sort(allocationTable.begin(), allocationTable.end());
            return reinterpret_cast<void*>(start);
        }

        std::cout << "Could not allocate " << size << " Byte for " << name << ".\n";
        return nullptr;
    }

    void myFree(void* ptr) {
        int start = reinterpret_cast<intptr_t>(ptr);

        // auto removeIt = std::remove_if(allocationTable.begin(), allocationTable.end(),
                                //    [start](const auto& entry) { return entry.first == start; });
        auto removeIt = std::find_if(allocationTable.begin(), allocationTable.end(),
                                 [start](const auto& entry) { return std::get<0>(entry) == start; });
        
        if (removeIt != allocationTable.end()) {
            for (int i = start; i < start + std::get<1>(*removeIt); ++i) {
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
            std::cout << "Name: " << std::get<2>(entry) << ", \tBaseAddress: " << std::get<0>(entry) << ", \tSize: " << std::get<1>(entry) << " Byte" << std::endl;
        }

        std::cout << "Memory free: " << std::endl;
        int startFree = -1;
        for (int i = 0; i < memorySize; ++i) {
            if (!memoryMap[i]) {
                if (startFree == -1) {
                    startFree = i;
                }
            } else {
                if (startFree != -1) {
                    std::cout << "BaseAddress: " << startFree << ", \tSize: " << i - startFree << " Byte" << std::endl;
                    startFree = -1;
                }
            }
        }

        if (startFree != -1) {
            std::cout << "BaseAddress: " << startFree << ", \tSize: " << memorySize - startFree << " Byte" << std::endl;
        }
    }
}; // MemoryManager






#endif