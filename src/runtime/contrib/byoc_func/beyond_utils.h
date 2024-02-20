#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <bitset>


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
void BydwriteValueToTxtFile(const T& value, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::app);

    if (outfile.is_open()) {
        outfile << value << std::endl;
        outfile.close();
    } else {
        std::cerr << "Unable to open the file: " << filename << std::endl;
    }
}
