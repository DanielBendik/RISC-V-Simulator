//******************************************************************
//
// Author: Daniel Bendik
// RISC-V Simulator
//
//******************************************************************

#include "hex.h"

/**
 * to_hex8(uint8_t i) formats an unsigned integer to its 2 byte hex equivalent.
 *
 * This is done through formatting the output stream 'os' using the std::hex
 * and std::setw modifiers to format it as a hexadecimal value, and the
 * std::setfill sets the leading value to a zero if 'i' is only a one byte value.
 *
 * @param i An unsigned 8-bit integer
 *
 * @return Returns a string object from the stream: This would be the 2 byte hex value.
 *
 * @note A static_cast is necessary to prevent the insertion operator from
 *       treating the 8-bit integer as a character.
 * 
 ********************************************************************************/

std::string hex::to_hex8(uint8_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
    return os.str();    
}

/**
 * to_hex32(uint32_t i) formats an unsigned integer to its 8 byte hex equivalent.
 *
 * This is done through formatting the output stream 'os' using the std::hex
 * and std::setw modifiers to format it as a hexadecimal value, and the
 * std::setfill sets the leading values to zeroes if 'i' is not the full 8 bytes.
 * std::setw is set to 8, as it is an 8 byte value to be displayed.
 *
 * @param i An unsigned 32-bit integer
 *
 * @return Returns a string object from the stream: This would be the 8 byte hex value.
 * 
 ********************************************************************************/

std::string hex::to_hex32(uint32_t i)
{
    std::ostringstream ous;
    ous << std::hex << std::setfill('0') << std::setw(8) << i;
    return ous.str();  
}

/**
 * to_hex0x32(uint32_t i) formats an unsigned integer to its 8 byte hex equivalent with a leading '0x'.
 *
 * This is done by concatenating the string value of "0x" and the to_hex32() function's 
 * return value, which is the 8 byte hex value.
 *
 * @param i An unsigned 32-bit integer
 *
 * @return Returns a string object from the stream: The concatenation of "0x" and the 8 byte hex value.
 * 
 ********************************************************************************/

std::string hex::to_hex0x32(uint32_t i)
{
    return std::string("0x")+to_hex32(i);
}

/**
 * to_hex0x20(uint32_t i) formats an unsigned integer to its 5 byte hex equivalent with a leading '0x'.
 *
 * This is done by concatenating the string value of "0x" and the to_hex20() function's 
 * return value, which is the 20 byte hex value. The function uses setw(5) to display this.
 * Used to format the lui and auipc instructions.
 *
 * @param i An unsigned 32-bit integer
 *
 * @return Returns a string object from the stream: The 5 byte hex value.
 * 
 ********************************************************************************/

std::string hex::to_hex0x20(uint32_t i)
{
    std::ostringstream oos;
    oos << std::hex << std::setfill('0') << std::setw(5) << i;
    return std::string("0x")+oos.str();  
}

/**
 * to_hex0x12(uint32_t i) formats an unsigned integer to its 3 byte hex equivalent with a leading '0x'.
 *
 * This is done by concatenating the string value of "0x" and the to_hex12() function's 
 * return value, which is the 12 byte hex value. The function uses setw(3) to display this.
 * Used to format the csrrX() instructions
 *
 * @param i An unsigned 32-bit integer
 *
 * @return Returns a string object from the stream: The 3 byte hex value.
 * 
 ********************************************************************************/

std::string hex::to_hex0x12(uint32_t i)
{
    std::ostringstream ops;
    ops << std::hex << std::setfill('0') << std::setw(3) << i;
    return std::string("0x")+ops.str();  
}