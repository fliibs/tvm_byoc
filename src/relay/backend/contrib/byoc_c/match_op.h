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






#endif