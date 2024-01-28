#include <iostream>
#include <bitset>

uint32_t RVVLoadInst(
    uint8_t vd,
    uint8_t rs1,
    uint8_t width = 0b110,
    uint8_t nf = 0,
    bool mew = false,
    uint8_t mop = 0,
    bool vm = true,
    uint8_t lumop = 0,
    uint8_t loadFP = 0b1011011
) {
    uint32_t instruction = 0;

    instruction |= (nf & 0x7) << 29;   // 3 bits for nf
    instruction |= (mew ? 1 : 0) << 28; // 1 bit for mew
    instruction |= (mop & 0x3) << 26;   // 2 bits for mop
    instruction |= (vm ? 1 : 0) << 25;  // 1 bit for vm
    instruction |= (lumop & 0x1F) << 20; // 5 bits for lumop
    instruction |= (rs1 & 0x1F) << 15;  // 5 bits for rs1
    instruction |= (width & 0x7) << 12; // 3 bits for width
    instruction |= (vd & 0x1F) << 7;    // 5 bits for vd
    instruction |= (loadFP & 0x7F);      // 7 bits for Load-FP

    return instruction;
}

uint32_t RVVStoreInst(
    uint8_t vs2,
    uint8_t vs1,
    uint8_t width = 0b110,
    uint8_t nf = 0,
    bool mew = false,
    uint8_t mop = 0,
    bool vm = true,
    uint8_t sumop = 0,
    uint8_t storeFP = 0b0101011
) {
    uint32_t instruction = 0;

    instruction |= (nf & 0x7) << 29;   // 3 bits for nf
    instruction |= (mew ? 1 : 0) << 28; // 1 bit for mew
    instruction |= (mop & 0x3) << 26;   // 2 bits for mop
    instruction |= (vm ? 1 : 0) << 25;  // 1 bit for vm
    instruction |= (sumop & 0x1F) << 20; // 5 bits for sumop
    instruction |= (vs1 & 0x1F) << 15;  // 5 bits for rs1
    instruction |= (width & 0x7) << 12; // 3 bits for width
    instruction |= (vs2 & 0x1F) << 7;    // 5 bits for vd
    instruction |= (storeFP & 0x7F);      // 7 bits for Store-FP

    return instruction;
}


uint32_t RVVArthInst(
    uint8_t funct6,
    uint8_t xs2,
    uint8_t xs1,
    uint8_t xd,
    uint8_t funct3,
    bool vm = true,
    uint8_t OP_V = 0b1011011
) {
    uint32_t instruction = 0;

    instruction |= (funct6 & 0x7) << 26;   // 6 bits for funct6
    instruction |= (vm ? 1 : 0) << 25;  // 1 bit for vm
    instruction |= (xs2 & 0x1F) << 20; // 5 bits for rs2
    instruction |= (xs1 & 0x1F) << 15;  // 5 bits for rs1
    instruction |= (funct3 & 0x7) << 12; // 3 bits for funct3
    instruction |= (xd & 0x1F) << 7;    // 5 bits for vd
    instruction |= (OP_V & 0x7F);      // 7 bits for OP-V

    return instruction;
}


uint32_t RVI32LoadInst(
    uint8_t vd,
    uint8_t rs1,
    uint8_t nf = 0,
    bool mew = false,
    uint8_t mop = 0,
    bool vm = true,
    uint8_t sumop = 0,
    uint8_t width = 0b110,
    uint8_t storeFP = 0b1011011
) {
    uint32_t instruction = 0;

    instruction |= (nf & 0x7) << 29;   // 3 bits for nf
    instruction |= (mew ? 1 : 0) << 28; // 1 bit for mew
    instruction |= (mop & 0x3) << 26;   // 2 bits for mop
    instruction |= (vm ? 1 : 0) << 25;  // 1 bit for vm
    instruction |= (sumop & 0x1F) << 20; // 5 bits for lumop
    instruction |= (rs1 & 0x1F) << 15;  // 5 bits for rs1
    instruction |= (width & 0x7) << 12; // 3 bits for width
    instruction |= (vd & 0x1F) << 7;    // 5 bits for vd
    instruction |= (storeFP & 0x7F);      // 7 bits for Load-FP

    return instruction;
}