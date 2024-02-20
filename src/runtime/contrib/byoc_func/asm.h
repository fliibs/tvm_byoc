#ifdef TARGET_MODE
    #define LOAD(reg, base, offset) \
        __asm__ ("movl %1, %0" : "=r"(reg) : "m"(*(base + offset)))

    #define MIN(result, x, y) \
        __asm__ volatile ( \
            "movl %1, %0\n\t" \
            "cmpl %2, %0\n\t" \
            "jge 1f\n\t" \
            "movl %2, %0\n\t" \
            "1:" \
            : "=r" (result) \
            : "r" (x), "r" (y))

    #define STORE(value, address) \
        __asm__ volatile ("movl %0, (%1)" : : "r" (value), "r" (address) : "memory")

    // RISC-V Vector Load Macro
    #define RVV_VLOAD(vd, rs1) \
        __asm__ volatile ("vle32.v %0, (%1)" : "=v" (vd) : "r" (rs1))

    #define RVV_VMIN(vd, vs2, rs1) \
        __asm__ volatile ("vmin.vx %0, %1, %2" : "=v" (vd) : "v" (vs2), "r" (rs1))

    #define RVV_VSTORE(vs3, rs1) \
        __asm__ volatile ("vse32.v %0, (%1)" : : "v" (vs3), "r" (rs1) : "memory")


#else
    #include <iostream>

    #define LOAD(reg, base, offset) \
        std::cout << "LOAD_OP: " << "__asm__ (\"movl %1, %0\" : \"=r\"(" << reg << ") : \"m\"(*(" << base << " + " << offset << ")))" << std::endl

    #define MIN(result, x, y) \
        std::cout << "MIN_OP: " << "__asm__ (\"movl %1, %0\\n\\t\" \\\n" \
                    "\"cmpl %2, %0\\n\\t\" \\\n" \
                    "\"jge 1f\\n\\t\" \\\n" \
                    "\"movl %2, %0\\n\\t\" \\\n" \
                    "1:\" : \"=r\"(" << result << ") : \"r\"(" << x << "), \"r\"(" << y << "))" << std::endl

    #define STORE(value, address) \
        std::cout << "STORE_OP: " << "__asm__ (\"movl %0, (%1)\" : : \"r\"(" << value << "), \"r\"(" << address << "))" << std::endl

    // RISC-V Vector Load Macro
    #define RVV_VLOAD(vd, rs1) \
        std::cout << "RVV_VLOAD: " << "__asm__ (\"vle32.v %0, (%1)\" : \"=v\"(" << vd << ") : \"r\"(" << rs1 << "))" << std::endl

    #define RVV_VMIN(vd, vs2, rs1) \
        std::cout << "RVV_VMIN: " << "__asm__ (\"vmin.vx %0, %1, %2\" : \"=v\"(" << vd << ") : \"v\"(" << vs2 << "), \"r\"(" << rs1 << "))" << std::endl

    #define RVV_VSTORE(vs3, rs1) \
        std::cout << "RVV_VSTORE: " << "__asm__ (\"vse32.v %0, (%1)\" : : \"v\"(" << vs3 << "), \"r\"(" << rs1 << "))" << std::endl


#endif
