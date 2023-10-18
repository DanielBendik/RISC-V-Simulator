//******************************************************************
//
// Author: Daniel Bendik
// RISC-V Simulator
//
//******************************************************************

#include "cpu_single_hart.h"

void cpu_single_hart::run(uint64_t exec_limit)
{
    uint64_t lmt = 0;
    
    if (exec_limit == 0)
    {
        while(!is_halted())
        {
            tick("");
        }
        
        if (is_halted() == true)
        {
            std::cout << "Execution terminated. Reason: " << get_halt_reason() << "\n"
                      << std::dec << get_insn_counter() << " instructions executed" << std::endl;
        }
    }
    else
    {
        while(!is_halted())
        {          
            tick("");

            lmt++;
            
            if (lmt == exec_limit-1)
            {
                set_show_registers(false);
            }

            if (lmt == exec_limit)
            {
                rv32i_hart::set_halt(true);
                
                if (is_halted() == true)
                {
                    

                    if (get_halt_reason() != "none")
                    {
                        std::cout << "Execution terminated. Reason: " << get_halt_reason() << "\n";
                    }
                    std::cout << std::dec << get_insn_counter() << " instructions executed" << std::endl;
                }

                break;
            }
        }

    }
}