//******************************************************************
//
// Author: Daniel Bendik
// RISC-V Simulator
//
//******************************************************************

#include "registerfile.h"

registerfile::registerfile()
{
    regVec.clear();
    regVec.resize(32, 0xf0f0f0f0);
    regVec.at(0)=0x00000000;
}

registerfile::~registerfile()
{
    
}

void registerfile::reset()
{
    registerfile();
}

void registerfile::set(uint32_t r, int32_t val)
{
    if (r != 0)
    {
        regVec.at(r) = val;
    }
}

int32_t registerfile::get(uint32_t r) const
{
    if (r == 0)
    {
        return 0;
    }
    else
    {
        return regVec[r];
    }
}

void registerfile::dump(const std::string &hdr) const
{
    for (int i = 0; i < 32; i++)    // Iterate through the entire "mem" vector
    {
        if (i % 4 == 0 && i % 8 != 0)      // Space between every 8 bytes
        {
            std::cout << ' ';
        }

        if (i % 8 == 0 && i != 0)
        {
            std::cout << std::endl;
        }

        if (i % 8 == 0)
        {
            std::stringstream ss;
            ss << 'x' << i;
            std::string str = ss.str();

            std::cout << hdr << std::setw(3) << std::setfill(' ') << std::right << str;
        }

        std::cout << ' ' << to_hex32(regVec[i]);    // Print out each register value
    }

    std::cout << std::endl;
}
