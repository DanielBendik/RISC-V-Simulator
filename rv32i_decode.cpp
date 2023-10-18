//******************************************************************
//
// Author: Daniel Bendik
// RISC-V Simulator
//
//******************************************************************

#include "rv32i_decode.h"

/**
 * Decodes / displays the hexadecimal instructions. 
 *
 * By running through each case using switch statements, the opcode
 * is further split into sub-types and then further divided by the 
 * values of the funct3 and funct7 bits. These determine what
 * instruction is being decoded from the mem (memory) vector. 
 *
 * @param addr Address of the instruction. 32 bits long.
 * @param insn Instruction to be decoded. 
 *
 * @return Returns a render() of the instruction: The appropriate
 *         mnemonic and any relevant values such as register values,
 *         imm_x's, and/or base displacement / address values. 
 *
 * @note Includes defaults and asserts so the function doesn't fall through.
 *
 * @warning Will read any insn, even if it is invalid. 
            Will return render_illegal_insn() in such case.
 *
 ********************************************************************************/

std::string rv32i_decode::decode(uint32_t addr, uint32_t insn) 
{
switch(get_opcode(insn))
{
    default: return render_illegal_insn();
    case opcode_lui: return render_lui(insn);
    case opcode_auipc: return render_auipc(insn);
    case opcode_jal: return render_jal(addr, insn);
    case opcode_jalr: return render_jalr(insn);

    case opcode_rtype:
        switch(get_funct3(insn))
        {
            default: return render_illegal_insn();
            
            case funct3_add:
                switch(get_funct7(insn))
                {
                    default: return render_illegal_insn();
                    case funct7_add: return render_rtype(insn, "add");
                    case funct7_sub: return render_rtype(insn, "sub");
                }
                assert(0 && "unrecognized funct3");
  
            case funct3_sll: return render_rtype(insn, "sll");
            case funct3_slt: return render_rtype(insn, "slt");
            case funct3_sltu: return render_rtype(insn, "sltu");
            case funct3_xor: return render_rtype(insn, "xor");

            case funct3_srx:
                switch(get_funct7(insn))
                {
                    default: return render_illegal_insn();
                    case funct7_srl: return render_rtype(insn, "srl");
                    case funct7_sra: return render_rtype(insn, "sra");
                }
                assert(0 && "unrecognized funct7");  

            case funct3_or: return render_rtype(insn, "or");
            case funct3_and: return render_rtype(insn, "and");
        }
        assert(0 && "unrecognized funct3");

    case opcode_btype:
        switch (get_funct3(insn))
        {
            default: return render_illegal_insn();
            case funct3_beq: return render_btype(addr, insn, "beq");
            case funct3_bne: return render_btype(addr, insn, "bne");
            case funct3_blt: return render_btype(addr, insn, "blt");
            case funct3_bge: return render_btype(addr, insn, "bge");
            case funct3_bltu: return render_btype(addr, insn, "bltu");
            case funct3_bgeu: return render_btype(addr, insn, "bgeu");
        }
        assert(0 && "unrecognized funct3"); // impossible

    case opcode_system:
        switch(get_funct3(insn))
        {
            default: return render_illegal_insn();
            case funct3_csrrw: return render_csrrx(insn, "csrrw");
            case funct3_csrrs: return render_csrrx(insn, "csrrs");
            case funct3_csrrc: return render_csrrx(insn, "csrrc");

            case funct3_csrrwi: return render_csrrxi(insn, "csrrwi");
            case funct3_csrrsi: return render_csrrxi(insn, "csrrsi");
            case funct3_csrrci: return render_csrrxi(insn, "csrrci");

            case funct3_e:
                switch(get_imm_i(insn))
                {
                    default: return render_illegal_insn();
                    case 0: return render_ecall();
                    case 1: return render_ebreak();
                }
                assert(0 && "unrecognized imm_i");
        }
        assert(0 && "unrecognized funct3");

    case opcode_stype:
        switch(get_funct3(insn))
        {
            default: return render_illegal_insn();
            case funct3_sb: return render_stype(insn, "sb");
            case funct3_sh: return render_stype(insn, "sh");
            case funct3_sw: return render_stype(insn, "sw");
        }
        assert(0 && "unrecognized funct3");

    case opcode_load_imm:
        switch(get_funct3(insn))
        {
            default: return render_illegal_insn();
            case funct3_lb: return render_itype_load(insn, "lb");
            case funct3_lh: return render_itype_load(insn, "lh");
            case funct3_lw: return render_itype_load(insn, "lw");
            case funct3_lbu: return render_itype_load(insn, "lbu");
            case funct3_lhu: return render_itype_load(insn, "lhu");
        }
        assert(0 && "unrecognized funct3");

    case opcode_alu_imm:
        switch (get_funct3(insn))
        {
            default: return render_illegal_insn();
            case funct3_add: return render_itype_alu(insn, "addi", get_imm_i(insn));
            case funct3_sll: return render_itype_alu(insn, "slli", get_imm_i(insn)%XLEN);
            case funct3_slt: return render_itype_alu(insn, "slti", get_imm_i(insn));
            case funct3_sltu: return render_itype_alu(insn, "sltiu", get_imm_i(insn));
            case funct3_xor: return render_itype_alu(insn, "xori", get_imm_i(insn));
            case funct3_or: return render_itype_alu(insn, "ori", get_imm_i(insn));
            case funct3_and: return render_itype_alu(insn, "andi", get_imm_i(insn));

            case funct3_srx:
                switch(get_funct7(insn))
                {
                    default: return render_illegal_insn();
                    case funct7_sra: return render_itype_alu(insn, "srai", get_imm_i(insn)%XLEN);
                    case funct7_srl: return render_itype_alu(insn, "srli", get_imm_i(insn)%XLEN);
                }
        assert(0 && "unrecognized funct7"); // impossible
        }
    assert(0 && "unrecognized funct3"); // impossible

}
assert(0 && "unrecognized opcode"); // It should be impossible to ever get here!
}

/**
 * get_opcode() gets the opcode from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the first (least significant) 7 bits of the insn.
 *
 ********************************************************************************/

uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
    return (insn & 0x0000007f);
}

/**
 * get_rd() gets the rd value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 5 bits of the insn to the left of the opcode.
 *
 ********************************************************************************/

uint32_t rv32i_decode::get_rd(uint32_t insn)
{
    int32_t rd = (insn & 0x00000f80) >> 7;  // Shift right 7 to get proper value
    return rd;
}

/**
 * get_funct3() gets the rd value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 3 bits of the insn to the left of the rd bits.
 *
 ********************************************************************************/

uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
    int32_t funct3 = (insn & 0x00007000) >> 12;  // Shift right 12 to get proper value
    return funct3;
}

/**
 * get_rs1() gets the rs1 value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 5 bits of the insn to the left of the funct3 bits.
 *
 ********************************************************************************/

uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
    int32_t rs1 = (insn & 0x000f8000) >> 15;  // Shift right 15 to get proper value
    return rs1;
}

/**
 * get_rs2() gets the rs2 value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 5 bits of the insn to the left of the rs1 bits.
 *
 ********************************************************************************/

uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
    int32_t rs2 = (insn & 0x01f00000) >> 20;  // Shift right 20 to get proper value
    return rs2;
}

/**
 * get_funct7() gets the funct7 value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 7 bits of the insn to the left of the rs2 or shamt bits.
 *
 ********************************************************************************/

uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
    int32_t funct7 = (insn & 0xfe000000) >> 25;  // Shift right 25 to get proper value
    return funct7;
}

/**
 * get_imm_i() gets the imm_i value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 12 bits of the insn to the left of the rs1 bits.
 *
 ********************************************************************************/

int32_t rv32i_decode::get_imm_i(uint32_t insn)
{
    int32_t imm_i = (insn & 0xfff00000) >> 20;  // Shift right 20 to get proper value
    
    if (insn & 0x80000000)
    {
        imm_i |= 0xfffff000;    // sign-extend the left
    }

    return imm_i;
}

/**
 * get_imm_u() gets the imm_u value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 20 bits of the insn to the left of the rd bits.
 *         Zeroes out the least significant 12 bits at the beginning.
 *
 ********************************************************************************/

int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
    int32_t imm_u = (insn & 0xfffff000);

    return imm_u;
}

/**
 * get_imm_b() gets the imm_b value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 13 bits of the insn that are all over the place.
 *         Sign extension.
 *
 ********************************************************************************/

int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
    int32_t imm_b = (insn & 0x7e000000) >> (25-5);  // b, c, d, e, f, g
    imm_b |= (insn & 0x00000f00) >> (8-1);    // u, v, w, x
    imm_b |= (insn & 0x80000000) >> (31-12);  // Most Significant Bit 'a'
    imm_b |= (insn & 0x00000080) << (11-7);   // Move 'y' to the left 4 positions

    if (insn & 0x80000000)
    {
        imm_b |= 0xffffe000; // sign-extend the left 19 bits
    }

    return imm_b;
}

/**
 * get_imm_s() gets the imm_s value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 12 bits of the insn that represent imm_s.
 *         Sign extension.
 *
 ********************************************************************************/

int32_t rv32i_decode::get_imm_s(uint32_t insn)
{
    int32_t imm_s = (insn & 0xfe000000) >> (25-5);  // abcdefg
    imm_s |= (insn & 0x00000f80) >> (7-0);          // uvwxy

    if (insn & 0x80000000)
    {
        imm_s |= 0xfffff000; // sign-extend the left
    }

    return imm_s;
}

/**
 * get_imm_j() gets the imm_j value from the insn passed. 
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns the 20 bits of the insn that represent imm_j.
 *         Sign extension for 11 bits.
 *
 ********************************************************************************/

int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
    int32_t imm_j = (insn & 0x000ff000);       // mnopqrst
    imm_j |= (insn & 0x7fe00000) >> (21-1);    // bcdefghijk
    imm_j |= (insn & 0x80000000) >> (31-20);   // a
    imm_j |= (insn & 0x00100000) >> (20-11);   // l
    
    if (insn & 0x80000000)
    {
        imm_j |= 0xffe00000;  // sign-extend the left
    }

    return imm_j;
}

/**
 * render_illegal_insn() prints an error message.
 *
 * @return Returns a string containing the error message.
 *
 ********************************************************************************/

std::string rv32i_decode::render_illegal_insn()
{
    return "ERROR: UNIMPLEMENTED INSTRUCTION";
}

/**
 * render_lui() renders info about the lui instruction.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the register and imm_u value.
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns a string displaying the lui instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_lui(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    std::ostringstream os;
    os << render_mnemonic("lui") << render_reg(rd) << ","
       << to_hex0x20((imm_u >> 12)&0x0fffff);

    return os.str();
}

/**
 * render_auipc() renders info about the auipc instruction.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the register and imm_u value.
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns a string displaying the auipc instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_auipc(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    std::ostringstream os;
    os << render_mnemonic("auipc") << render_reg(rd) << ","
       << to_hex0x20((imm_u >> 12)&0x0fffff);

    return os.str();
}

/**
 * render_jal() renders info about the jal instruction.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the register and imm_j value.
 *
 * @param addr Address of where the instruction is in the mem vector.
 * @param insn Instruction to be decoded. 
 *
 * @return Returns a string displaying the jal instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_j = get_imm_j(insn);
    imm_j += addr;

    std::ostringstream os;
    os << render_mnemonic("jal") << render_reg(rd) << ","
       << to_hex0x32(imm_j);

    return os.str();
}

/**
 * render_jalr() renders info about the jalr instruction.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the register and
 * the base-displacement formatted value.
 *
 * @param insn Instruction to be decoded. 
 *
 * @return Returns a string displaying the jalr instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_jalr(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    uint32_t imm_i = get_imm_i(insn);
    uint32_t rs1 = get_rs1(insn);

    std::ostringstream os;
    os << render_mnemonic("jalr") << render_reg(rd) << "," 
       << render_base_disp(imm_i, rs1);

    return os.str();
}

/**
 * render_itype_alu() renders info about the I-Type / alu instructions.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the registers and imm_i value.
 * Renders the mnemonic passed, due to the encompassing nature of this function.
 *
 * @param *mnemonic Char[] value of the instruction.
 * @param insn Instruction to be decoded. 
 * @param imm_i The value of the imm_i bits of the I-Type instruction. 
 *
 * @return Returns a string displaying the I_Type / alu instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << ","
       << render_reg(rs1) << "," << imm_i;

    return os.str();
}

/**
 * render_itype_load() renders info about the I-Type / load instructions.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the registers and imm_i value
 * to render the base-displacement format.
 *
 * @param *mnemonic Char[] value of the instruction.
 * @param insn Instruction to be decoded. 
 *
 * @return Returns a string displaying the I-Type / load instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_itype_load(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t imm_i = get_imm_i(insn);
    uint32_t rs1 = get_rs1(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << ","
       << render_base_disp(imm_i, rs1);

    return os.str();
}

/**
 * render_stype() renders info about the S-Type instructions.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the register value
 * of rs2, and rs1 and imm_s value to render the base-displacement format.
 *
 * @param *mnemonic Char[] value of the instruction.
 * @param insn Instruction to be decoded. 
 *
 * @return Returns a string displaying the S-Type instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_stype(uint32_t insn, const char *mnemonic)
{
    uint32_t imm_s = get_imm_s(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rs2) << ","
       << render_base_disp(imm_s, rs1);

    return os.str();
}

/**
 * render_btype() renders info about the B-Type instructions.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the register values
 * of rs1 and rs2, and the pcrel_13 value to render the address of
 * the next instruction.
 *
 * @param *mnemonic Char[] value of the instruction.
 * @param insn Instruction to be decoded.
 * @param addr Address of the instruction.
 *
 * @return Returns a string displaying the B-Type instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char *mnemonic)
{
    uint32_t pcrel_13 = get_imm_b(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    int32_t pcrel_signed = pcrel_13;
 
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rs1) << ","
       << render_reg(rs2) << "," << to_hex0x32(addr+pcrel_signed);

    return os.str();
}

/**
 * render_rtype() renders info about the R-Type instructions.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the register values
 * of rs1, rs2, and the rd value to render them.
 *
 * @param *mnemonic Char[] value of the instruction.
 * @param insn Instruction to be decoded.
 *
 * @return Returns a string displaying the R-Type instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_rtype(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," 
       << render_reg(rs1) << "," << render_reg(rs2);

    return os.str();
}

/**
 * render_csrrx() renders info about the Csrrx-Type instructions.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the register values
 * of rs1, rd, and the imm_i value to render the 3 byte value.
 *
 * @param *mnemonic Char[] value of the instruction.
 * @param insn Instruction to be decoded.
 *
 * @return Returns a string displaying the Csrrx-Type instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_csrrx(uint32_t insn, const char *mnemonic)
{
    uint32_t imm_i = get_imm_i(insn);
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," 
       << to_hex0x12((imm_i&0xfff)) << "," << render_reg(rs1);

    return os.str();
}

/**
 * render_csrrxi() renders info about the Csrrxi-Type instructions.
 *
 * Uses get() functions to grab the necessary values to be 
 * able to print out the information, like the register values
 * of rs1 / zimm, rd, and the imm_i value to render the 3 byte value.
 *
 * @param *mnemonic Char[] value of the instruction.
 * @param insn Instruction to be decoded.
 *
 * @return Returns a string displaying the Csrrxi-Type instruction information.
 *
 ********************************************************************************/

std::string rv32i_decode::render_csrrxi(uint32_t insn, const char *mnemonic)
{
    uint32_t imm_i = get_imm_i(insn);
    uint32_t rd = get_rd(insn);
    uint32_t zimm = get_rs1(insn);

    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << "," 
       << to_hex0x12((imm_i&0xfff)) << "," << zimm;
    return os.str();
}

/**
 * render_base_disp() renders the base-displacement formatted values. 
 *
 * Renders the base register value as well as the displacement register. 
 *
 * @param base Base register.
 * @param disp Displacement register.
 *
 * @return Returns a string displaying the base displacement format.
 *
 ********************************************************************************/

std::string rv32i_decode::render_base_disp(uint32_t base, int32_t disp)
{
    int32_t signedBase = base;

    std::stringstream ss;
    ss << signedBase << '(' << render_reg(disp) << ')';
    std::string str = ss.str();
    return str;
}

/**
 * render_ebreak() renders the value "ebreak". 
 *
 * @return Returns "ebreak".
 *
 ********************************************************************************/

std::string rv32i_decode::render_ebreak()
{
    return "ebreak";
}

/**
 * render_ecall() renders the value "ecall". 
 *
 * @return Returns "ecall".
 *
 ********************************************************************************/

std::string rv32i_decode::render_ecall()
{
    return "ecall";
}

/**
 * render_mnemonic() formats the mnemonic passed in.
 *
 * @return Returns the left-aligned mnemonic, formatted by 
 *         setw() using mnemonic_width, as a string.
 *
 ********************************************************************************/

std::string rv32i_decode::render_mnemonic(const std::string &m)
{
    std::stringstream ss;
    ss << std::setw(mnemonic_width) << std::left << m;
    std::string str = ss.str();
    return str;
}

/**
 * render_reg() formats the integer value of a register to a string.
 *
 * @return Returns the string value of 'x' followed by the register value.
 *
 ********************************************************************************/

std::string rv32i_decode::render_reg(int r)
{
    std::stringstream ss;
    ss << 'x' << r;
    std::string str = ss.str();
    return str;
}
