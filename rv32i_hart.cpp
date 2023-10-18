//******************************************************************
//
// Author: Daniel Bendik
// RISC-V Simulator
//
//******************************************************************

#include "rv32i_hart.h"

void rv32i_hart::reset()
{
    pc = 0;
    regs.reset();
    regs.set(2, mem.get_size());
    insn_counter = 0;
    halt = false;
    halt_reason = "none";
}

void rv32i_hart::dump(const std::string &hdr) const
{
    regs.dump(hdr);

    std::cout << " pc " << to_hex32(pc) << std::endl;
}

void rv32i_hart::tick(const std::string &hdr)
{
    if (pc % 4 != 0)
    {
        halt = true;
        halt_reason = "PC alignment error";  // Alignment error check
        return;
    }

    insn_counter++;

    uint32_t getinsn = mem.get32(pc);  // Get the instruction from mem

    if (show_instructions == true && show_registers == true)
    {


        exec(getinsn, &std::cout);      // Shows instructions and registers
        std::cout << std::endl;
        
        if (!halt)
        {
            regs.dump(hdr);
            std::cout << " pc " << to_hex32(pc) << std::endl;
        }
    }
    else if (show_instructions == true)
    {
        exec(getinsn, &std::cout);      // Only shows instructions
        std::cout << std::endl;
    }
    else
    {
        exec(getinsn, nullptr);
    }
}

void rv32i_hart::exec(uint32_t insn, std::ostream* pos)
{
    switch(get_opcode(insn))
    {
        default: exec_illegal_insn(pos); return;
        case opcode_lui: exec_lui(insn, pos); return;
        case opcode_auipc: exec_auipc(insn, pos); return;
        case opcode_jal: exec_jal(insn, pos); return;
        case opcode_jalr: exec_jalr(insn, pos); return;

        case opcode_rtype:
            switch(get_funct3(insn))
            {
                default: exec_illegal_insn(pos); return;

                case funct3_add:
                    switch(get_funct7(insn))
                    {
                        default: exec_illegal_insn(pos); return;
                        case funct7_add: exec_add(insn, pos); return;
                        case funct7_sub: exec_sub(insn, pos); return;
                    }

                case funct3_sll: exec_sll(insn, pos); return;
                case funct3_slt: exec_slt(insn, pos); return;
                case funct3_sltu: exec_sltu(insn, pos); return;
                case funct3_xor: exec_xor(insn, pos); return;

                case funct3_srx:
                    switch(get_funct7(insn))
                    {
                        default: exec_illegal_insn(pos); return;
                        case funct7_srl: exec_srl(insn, pos); return;
                        case funct7_sra: exec_sra(insn, pos); return;
                    }

                case funct3_or: exec_or(insn, pos); return;
                case funct3_and: exec_and(insn, pos); return;
            }

        case opcode_btype:
            switch (get_funct3(insn))
            {
                default: exec_illegal_insn(pos); return;
                case funct3_beq: exec_beq(insn, pos); return;
                case funct3_bne: exec_bne(insn, pos); return;
                case funct3_blt: exec_blt(insn, pos); return;
                case funct3_bge: exec_bge(insn, pos); return;
                case funct3_bltu: exec_bltu(insn, pos); return;
                case funct3_bgeu: exec_bgeu(insn, pos); return;
            }

        case opcode_system:
            switch(get_funct3(insn))
            {
                default: exec_illegal_insn(pos); return;

                case funct3_csrrs: exec_csrrs(insn, pos); return;

                case funct3_e:
                    switch(get_imm_i(insn))
                    {
                        default: exec_illegal_insn(pos); return;
                        case 1: return exec_ebreak(insn, pos); return;
                        case 0: return exec_ecall(pos); return;
                    }
            }

        case opcode_stype:
            switch(get_funct3(insn))
            {
                default: exec_illegal_insn(pos); return;
                case funct3_sb: exec_sb(insn, pos); return;
                case funct3_sh: exec_sh(insn, pos); return;
                case funct3_sw: exec_sw(insn, pos); return;
            }

        case opcode_alu_imm:
            switch (get_funct3(insn))
            {
                default: exec_illegal_insn(pos); return;
                case funct3_add: exec_addi(insn, pos); return;
                case funct3_sll: exec_slli(insn, pos); return;
                case funct3_slt: exec_slti(insn, pos); return;
                case funct3_sltu: exec_sltiu(insn, pos); return;
                case funct3_xor: exec_xori(insn, pos); return;
                case funct3_or: exec_ori(insn, pos); return;
                case funct3_and: exec_andi(insn, pos); return;

                case funct3_srx:
                    switch(get_funct7(insn))
                    {
                        default: exec_illegal_insn(pos); return;
                        case funct7_sra: exec_srai(insn, pos); return;
                        case funct7_srl: exec_srli(insn, pos); return;
                    }
            }

        case opcode_load_imm:
            switch(get_funct3(insn))
            {
                default: exec_illegal_insn(pos); return;
                case funct3_lbu: exec_lbu(insn, pos); return;
                case funct3_lhu: exec_lhu(insn, pos); return;
                case funct3_lb: exec_lb(insn, pos); return;
                case funct3_lh: exec_lh(insn, pos); return;
                case funct3_lw: exec_lw(insn, pos); return;
            }

    }
}

void rv32i_hart::exec_illegal_insn(std::ostream* pos)
{
    if (pos)
    {
        *pos << render_illegal_insn();
    }
    halt = true;
    halt_reason = "Illegal instruction";
}

void rv32i_hart::exec_ebreak(int32_t insn, std::ostream* pos)
{
    if (pos)
    {
        std::string s = render_ebreak();
        *pos << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// HALT";
    }

    halt = true;
    halt_reason = "EBREAK instruction";
}

void rv32i_hart::exec_ecall(std::ostream* pos)
{
    if (pos)
    {
        std::string s = render_ecall();
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// ECALL";
    }

    halt = true;
    halt_reason = "ECALL instruction";
}

void rv32i_hart::exec_lui(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t immu = get_imm_u(insn);

    if (pos)
    {
        std::string s = render_lui(insn);
        *pos << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(immu);
    }

    regs.set(rd, get_imm_u(insn));
    pc += 4;
}

void rv32i_hart::exec_auipc(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t immu = get_imm_u(insn);
    int32_t val = pc + immu;

    if (pos)
    {
        std::string s = render_auipc(insn);
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(pc) << " + "
             << hex::to_hex0x32(immu) << " = " << hex::to_hex0x32(val); 
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_jal(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t immj = get_imm_j(insn);
    int32_t val = pc + immj;

    if (pos)
    {
        std::string s = render_jal(pc, insn);
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(pc+4) << ",  pc = "
             << to_hex0x32(pc) << " + " << to_hex0x32(immj) << " = " << to_hex0x32(val);

    }

    regs.set(rd, pc+4);
    pc = val;
}

void rv32i_hart::exec_jalr(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t immi = get_imm_i(insn);
    uint32_t val = ((regs.get(rs1) + immi) & 0xfffffffe);

    if (pos)
    {
        std::string s = render_jalr(insn);
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << to_hex0x32(pc+4) << ",  pc = ("
             << to_hex0x32(immi) << " + " << to_hex0x32(regs.get(rs1)) 
             << ") & 0xfffffffe = " << to_hex0x32(val);
    }

    regs.set(rd, pc+4);
    pc = val;
}

void rv32i_hart::exec_beq(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t immb = get_imm_b(insn);
    int32_t val;

    if (regs.get(rs1) == regs.get(rs2))
    {
        val = immb;
    }
    else
    {
        val = 4;
    }

    if (pos)
    {
        std::string s = render_btype(pc, insn, "beq     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << to_hex0x32(regs.get(rs1)) << " == "
             << to_hex0x32(regs.get(rs2)) << " ? " << to_hex0x32(immb)
             << " : 4) = " << to_hex0x32(pc+val);
    }
    pc += val;
}

void rv32i_hart::exec_bne(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t immb = get_imm_b(insn);
    int32_t val;

    if (regs.get(rs1) != regs.get(rs2))
    {
        val = immb;
    }
    else
    {
        val = 4;
    }

    if (pos)
    {
        std::string s = render_btype(pc, insn, "bne     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << to_hex0x32(regs.get(rs1)) << " != "
             << to_hex0x32(regs.get(rs2)) << " ? " << to_hex0x32(immb)
             << " : 4) = " << to_hex0x32(pc+val);

    }
    pc += val;
}

void rv32i_hart::exec_blt(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t immb = get_imm_b(insn);
    int32_t val;

    if (regs.get(rs1) < regs.get(rs2))
    {
        val = immb;
    }
    else
    {
        val = 4;
    }

    if (pos)
    {
        std::string s = render_btype(pc, insn, "blt     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << to_hex0x32(regs.get(rs1)) << " < "
             << to_hex0x32(regs.get(rs2)) << " ? " << to_hex0x32(immb)
             << " : 4) = " << to_hex0x32(pc+val);
    }
    pc += val;
}

void rv32i_hart::exec_bge(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t immb = get_imm_b(insn);
    int32_t val;

    if (regs.get(rs1) >= regs.get(rs2))
    {
        val = immb;
    }
    else
    {
        val = 4;
    }

    if (pos)
    {
        std::string s = render_btype(pc, insn, "bge     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << to_hex0x32(regs.get(rs1)) << " >= "
             << to_hex0x32(regs.get(rs2)) << " ? " << to_hex0x32(immb)
             << " : 4) = " << to_hex0x32(pc+val);
    }

    pc += val;
}

void rv32i_hart::exec_bltu(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t immb = get_imm_b(insn);
    uint32_t val;

    uint32_t first = regs.get(rs1);
    uint32_t second = regs.get(rs2);

    if (first < second)
    {
        val = immb;
    }
    else
    {
        val = 4;
    }

    if (pos)
    {
        std::string s = render_btype(pc, insn, "bltu    ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << to_hex0x32(regs.get(rs1)) << " <U "
             << to_hex0x32(regs.get(rs2)) << " ? " << to_hex0x32(immb)
             << " : 4) = " << to_hex0x32(pc+val);
    }
    pc += val;
}

void rv32i_hart::exec_bgeu(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t immb = get_imm_b(insn);
    uint32_t val;

    uint32_t first = regs.get(rs1);
    uint32_t second = regs.get(rs2);

    if (first >= second)
    {
        val = immb;
    }
    else
    {
        val = 4;
    }

    if (pos)
    {
        std::string s = render_btype(pc, insn, "bgeu    ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << to_hex0x32(regs.get(rs1)) << " >=U "
             << to_hex0x32(regs.get(rs2)) << " ? " << to_hex0x32(immb)
             << " : 4) = " << to_hex0x32(pc+val);
    }
    pc += val;
}

void rv32i_hart::exec_addi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t immi = get_imm_i(insn);
    int32_t val = (regs.get(rs1) + immi);

    if (pos)
    {
        std::string s = render_itype_alu(insn, "addi    ", get_imm_i(insn));
        *pos << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(immi) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lbu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t immi = get_imm_i(insn);
    uint8_t val = mem.get8(regs.get(rs1)+immi)&0x000000ff;

    if (pos)
    {
        std::string s = render_itype_load(insn, "lbu     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = zx(m8(" << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(immi) << ")) = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lhu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t immi = get_imm_i(insn);
    uint16_t val = mem.get16(regs.get(rs1)+immi)&0x0000ffff;

    if (pos)
    {
        std::string s = render_itype_load(insn, "lhu     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = zx(m16(" << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(immi) << ")) = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lb(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t immi = get_imm_i(insn);             // signed
    int8_t val = mem.get8(regs.get(rs1)+immi);  // signed

    if (pos)
    {
        std::string s = render_itype_load(insn, "lb      ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = sx(m8(" << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(immi) << ")) = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lh(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t immi = get_imm_i(insn);               // signed
    int16_t val = mem.get16(regs.get(rs1)+immi);  // signed

    if (pos)
    {
        std::string s = render_itype_load(insn, "lh      ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = sx(m16(" << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(immi) << ")) = " << hex::to_hex0x32(val);
    }
    
    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_lw(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t immi = get_imm_i(insn);
    uint32_t val = mem.get32(regs.get(rs1)+immi);

    if (pos)
    {
        std::string s = render_itype_load(insn, "lw      ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = sx(m32(" << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(immi) << ")) = " << hex::to_hex0x32(val);

    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sb(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t imms = get_imm_s(insn);
    uint32_t val = regs.get(rs1)+imms;
    mem.set8(val, regs.get(rs2)&0x000000ff);

    if (pos)
    {
        std::string s = render_stype(insn, "sb      ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// m8(" << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(imms) << ") = " << hex::to_hex0x32(mem.get8(val));
    }

    pc += 4;
}

void rv32i_hart::exec_sh(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t imms = get_imm_s(insn);
    uint32_t val = regs.get(rs1)+imms;
    mem.set16(val, regs.get(rs2)&0x0000ffff);

    if (pos)
    {
        std::string s = render_stype(insn, "sh      ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// m16(" << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(imms) << ") = " << hex::to_hex0x32(mem.get16(val));
    }

    pc += 4;
}

void rv32i_hart::exec_sw(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t imms = get_imm_s(insn);
    uint32_t val = regs.get(rs1)+imms;
    mem.set32(val, regs.get(rs2));

    if (pos)
    {
        std::string s = render_stype(insn, "sw      ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// m32(" << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(imms) << ") = " << hex::to_hex0x32(mem.get32(val));
    }

    pc += 4;
}

void rv32i_hart::exec_slti(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t immi = get_imm_i(insn);

    int32_t val = (regs.get(rs1) < immi) ? 1 : 0;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "slti    ", get_imm_i(insn));
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(regs.get(rs1)) << " < "
             << std::dec << immi << ") ? 1 : 0 = " << hex::to_hex0x32(val);
        
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sltiu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    uint32_t immi = get_imm_i(insn);
    uint32_t rs1u = regs.get(rs1);

    int32_t val = (rs1u < immi) ? 1 : 0;

    if (pos)
    {
        std::string s = render_itype_alu(insn, "sltiu   ", get_imm_i(insn));
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(regs.get(rs1)) << " <U "
             << std::dec << immi << ") ? 1 : 0 = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_xori(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    uint32_t immi = get_imm_i(insn);

    uint32_t val = (regs.get(rs1) ^ immi);

    if (pos)
    {
        std::string s = render_itype_alu(insn, "xori    ", get_imm_i(insn));
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " ^ "
             << hex::to_hex0x32(immi) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_ori(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    uint32_t immi = get_imm_i(insn);

    uint32_t val = (regs.get(rs1) | immi);

    if (pos)
    {
        std::string s = render_itype_alu(insn, "ori     ", get_imm_i(insn));
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " | "
             << hex::to_hex0x32(immi) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_andi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    uint32_t immi = get_imm_i(insn);

    uint32_t val = (regs.get(rs1) & immi);

    if (pos)
    {
        std::string s = render_itype_alu(insn, "andi    ", get_imm_i(insn));
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " & "
             << hex::to_hex0x32(immi) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_slli(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    uint32_t immi = get_imm_i(insn);
    uint32_t shamt_i = (immi & 0x0000001f);
    uint32_t immiShift = (regs.get(rs1) << shamt_i);

    if (pos)
    {
        std::string s = render_itype_alu(insn, "slli    ", get_imm_i(insn));
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " << "
             << shamt_i << " = " << hex::to_hex0x32(immiShift);
    }

    regs.set(rd, immiShift);
    pc += 4;
}

void rv32i_hart::exec_srli(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    uint32_t immi = get_imm_i(insn);
    uint32_t shamt_i = (immi & 0x0000001f);
    uint32_t rs1u = regs.get(rs1);
    uint32_t immiShift = (rs1u >> shamt_i);

    if (pos)
    {
        std::string s = render_itype_alu(insn, "srli    ", get_imm_i(insn));
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> "
             << std::dec << shamt_i << " = " << hex::to_hex0x32(immiShift);
    }

    regs.set(rd, immiShift);
    pc += 4;
}

void rv32i_hart::exec_srai(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    uint32_t immi = get_imm_i(insn);
    uint32_t shamt_i = (immi & 0x0000001f);
    uint32_t immiShift = (regs.get(rs1) >> shamt_i);

    if (pos)
    {
        std::string s = render_itype_alu(insn, "srai    ", get_imm_i(insn)%XLEN);
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> "
             << std::dec << shamt_i << " = " << hex::to_hex0x32(immiShift);
    }

    regs.set(rd, immiShift);
    pc += 4;
}

void rv32i_hart::exec_add(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t val = (regs.get(rs1) + regs.get(rs2));

    if (pos)
    {
        std::string s = render_rtype(insn, "add     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " + "
             << hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sub(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t val = (regs.get(rs1) - regs.get(rs2));

    if (pos)
    {
        std::string s = render_rtype(insn, "sub     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " - "
             << hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sll(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    uint32_t rs2Shifted = (regs.get(rs2) & 0x0000001f);
    uint32_t sllShift = (regs.get(rs1) << rs2Shifted);

    if (pos)
    {
        std::string s = render_rtype(insn, "sll     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " << "
             << rs2Shifted << " = " << hex::to_hex0x32(sllShift);
    }

    regs.set(rd, sllShift);
    pc += 4;
}

void rv32i_hart::exec_slt(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);

    int32_t val = (regs.get(rs1) < regs.get(rs2)) ? 1 : 0;

    if (pos)
    {
        std::string s = render_rtype(insn, "slt     ");
        *pos << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(regs.get(rs1))
             << " < " << hex::to_hex0x32(regs.get(rs2)) << ") ? 1 : 0 = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_sltu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1U = regs.get(rs1);
    uint32_t rs2U = regs.get(rs2);

    int32_t val = (rs1U < rs2U) ? 1 : 0;

    if (pos)
    {
        std::string s = render_rtype(insn, "sltu    ");
        *pos << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1U)
             << " <U " << hex::to_hex0x32(rs2U) << ") ? 1 : 0 = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_xor(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1U = regs.get(rs1);
    uint32_t rs2U = regs.get(rs2);

    uint32_t val = (rs1U ^ rs2U);

    if (pos)
    {
        std::string s = render_rtype(insn, "xor     ");
        *pos << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1U)
             << " ^ " << hex::to_hex0x32(rs2U) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_srl(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1U = regs.get(rs1);
    uint32_t rs2U = regs.get(rs2);

    uint32_t rs2Shifted = (rs2U & 0x0000001f);

    uint32_t rs1Shift = (rs1U >> rs2Shifted);

    if (pos)
    {
        std::string s = render_rtype(insn, "srl     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> "
             << rs2Shifted << " = " << hex::to_hex0x32(rs1Shift);
    }

    regs.set(rd, rs1Shift);
    pc += 4;
}

void rv32i_hart::exec_sra(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    int32_t rs1U = regs.get(rs1);
    uint32_t rs2U = regs.get(rs2);

    uint32_t rs2Shifted = (rs2U & 0x0000001f);

    uint32_t rs1Shift = (rs1U >> rs2Shifted);

    if (pos)
    {
        std::string s = render_rtype(insn, "sra     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1U) << " >> "
             << rs2Shifted << " = " << hex::to_hex0x32(rs1Shift);
    }

    regs.set(rd, rs1Shift);
    pc += 4;
}

void rv32i_hart::exec_or(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1U = regs.get(rs1);
    uint32_t rs2U = regs.get(rs2);

    uint32_t val = (rs1U | rs2U);

    if (pos)
    {
        std::string s = render_rtype(insn, "or      ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1U) << " | "
             << hex::to_hex0x32(rs2U) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_and(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rs1U = regs.get(rs1);
    uint32_t rs2U = regs.get(rs2);

    uint32_t val = (rs1U & rs2U);
    
    if (pos)
    {
        std::string s = render_rtype(insn, "and     ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1U) << " & "
             << hex::to_hex0x32(rs2U) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val);
    pc += 4;
}

void rv32i_hart::exec_csrrs(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t csrrs = (get_imm_i(insn) & 0x00000fff);

    if (rs1 != 0 || csrrs != 0x00000f14)
    {
        halt = true;
        halt_reason = "Illegal CSR in CSRRS instruction";
    }
    
    if (pos)
    {
        std::string s = render_csrrx(insn, "csrrs   ");
        *pos << to_hex32(pc) << ": " << to_hex32(insn) << "  ";
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << mhartid;
    }

    regs.set(rd, mhartid);
    pc += 4;
}
