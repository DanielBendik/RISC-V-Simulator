//******************************************************************
//
// Author: Daniel Bendik
// RISC-V Simulator
//
//******************************************************************

#ifndef H_HEX 
#define H_HEX

#include <sstream>
#include <string>
#include <iomanip>

class hex
{
public:
    static std::string to_hex8(uint8_t i);
    static std::string to_hex32(uint32_t i);
    static std::string to_hex0x32(uint32_t i);
    static std::string to_hex0x20(uint32_t i);
    static std::string to_hex0x12(uint32_t i);
};

#endif