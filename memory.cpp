//******************************************************************
//
// Author: Daniel Bendik
// RISC-V Simulator
//
//******************************************************************

#include "memory.h"

/**
 * memory() initializes every byte to 0xa5 in the "mem" vector.
 *
 * "siz" amount of bytes are allocated to the "mem" vector and
 * "siz" is rounded up using the 'and' operator with bit manipulation.
 *
 * @param siz The amount of bytes representing memory size.
 *
 ********************************************************************************/

memory::memory(uint32_t siz)
{
    siz = (siz+15)&0xfffffff0;
    
    mem.resize(siz, 0xa5);
}

/**
 * ~memory() is the deconstructor for memory().
 *
 ********************************************************************************/

memory::~memory()
{

}

/**
 * check_illegal(uint32_t i) is a validity check for addresses.
 *
 * This function returns true if the address passed does not
 * represent an element in the "mem" vector. Prints out a warning
 * to std::cout showing which address is invalid.
 *
 * @param i Unsigned 32 bit integer representing an address value.
 *
 * @return True if invalid, false if valid address.
 *
 ********************************************************************************/

bool memory::check_illegal(uint32_t i) const
{
    if (i > mem.size())
    {
        std::cout << "WARNING: Address out of range: " << to_hex0x32(i) << std::endl;

        return true;
    }

    return false;
}

/**
 * get_size() const
 *
 * @return Returns the size of the "mem" vector, 
 *         represented by an unsigned 32-bit integer.
 *
 ********************************************************************************/

uint32_t memory::get_size() const
{
    return mem.size();
}

/**
 * get8(uint32_t addr)
 *
 * This function returns the value of the byte at the "addr" address. 
 * Checks for invalid address as a safety check.
 *
 * @param addr Unsigned 32 bit integer representing an address value.
 *
 * @return 0 if illegal address, value of the byte if legal address.
 *
 ********************************************************************************/

uint8_t memory::get8(uint32_t addr) const
{
    if (check_illegal(addr))  // If address is illegal,
    {
        return 0;             // Return 0.
    }
    else                      // If it is legal,
    {
        return mem[addr];     // Return the value of the byte at this address.
    }
}

/**
 * get16(uint32_t addr)
 *
 * This function returns the value of the 2 bytes at the "addr" address. 
 * This function calls get8() twice and combines them in little-endian
 * order to create a 16 bit return value.
 *
 * @param addr Unsigned 32 bit integer representing an address value.
 *
 * @return The two combined bytes representing a 16 bit value from the address specified.
 *
 ********************************************************************************/

uint16_t memory::get16(uint32_t addr) const
{
    uint8_t first = get8(addr);          // Get first byte

    addr += 1;                           // Increment addr to get next value

    uint8_t second = get8(addr);         // Get second byte that's next to the first

    uint16_t combined = (second << 8) | first;      // Shift 8 bits and 'combine'

    return combined;
}

/**
 * get32(uint32_t addr)
 *
 * This function returns the value of the 4 bytes at the "addr" address. 
 * This function calls get16() twice and combines them in little-endian
 * order to create a 32 bit return value.
 *
 * @param addr Unsigned 32 bit integer representing an address value.
 *
 * @return The four combined bytes representing a 32 bit value from the address specified.
 *
 ********************************************************************************/

uint32_t memory::get32(uint32_t addr) const
{
    uint16_t first = get16(addr);   // Get first value at "addr"

    addr += 2;                      // Move over two bytes since 16 bits are being combined

    uint16_t second = get16(addr);  // Get second value at "addr"

    uint32_t combined = (second << 16) | first;  // Shift 16 bits and 'combine' the two bit values

    return combined;
}

/**
 * get8_sx(uint32_t addr)
 *
 * This function calls the get8() function and returns a sign-extended value
 * of the byte as a 32-bit signed integer using bit manipulation.
 *
 * @param addr Unsigned 32 bit integer representing an address value.
 *
 * @return Sign extended value of the 8 bit value at "addr".
 *
 ********************************************************************************/

int32_t memory::get8_sx(uint32_t addr) const
{
    int32_t extendEight = get8(addr);

    extendEight = (extendEight << 24);  // Remove the 24 leading zeroes (it's an 8 bit value)
    extendEight = (extendEight >> 24);  // Sign extension 

    return extendEight;
}

/**
 * get16_sx(uint32_t addr)
 *
 * This function calls the get16() function and returns a sign-extended value
 * of the byte as a 32-bit signed integer using bit manipulation.
 *
 * @param addr Unsigned 32 bit integer representing an address value.
 *
 * @return Sign extended value of the 16 bit value at "addr".
 *
 ********************************************************************************/

int32_t memory::get16_sx(uint32_t addr) const
{
    int32_t extendSixteen = get16(addr);

    extendSixteen = (extendSixteen << 16);  // Remove the 16 leading zeroes
    extendSixteen = (extendSixteen >> 16);  // Sign extension

    return extendSixteen;
}

/**
 * get32_sx(uint32_t addr)
 *
 * This function just returns the value of the address passed.
 *
 * @param addr Unsigned 32 bit integer representing an address value.
 *
 * @return Technically, the 'sign extended' value of the 32 bit value at "addr".
 *
 ********************************************************************************/

int32_t memory::get32_sx(uint32_t addr) const
{
    return get32(addr);
}

/**
 * set8(uint32_t addr, uint8_t val) sets values in the "mem" vector.
 *
 * This function checks if the address is valid, and then sets the value
 * at the specified address "addr" to whatever value "val" is specified as.
 *
 * @param addr Unsigned 32 bit integer representing an address value.
 * @param val Unsigned 8 bit integer representing a value to put into the memory.
 *
 ********************************************************************************/

void memory::set8(uint32_t addr, uint8_t val)
{
    if (check_illegal(addr))  // If address is illegal,
    {
        return;               // Exit.
    }
    else                      // If it is legal,
    {
        mem.at(addr) = val;   // Set the value at this addr to val.
    }
}

/**
 * set16(uint32_t addr, uint16_t val) sets values in the "mem" vector.
 *
 * This function uses bit manipulation to prepare the bit values 
 * and calls set8() twice to set the values in the memory in the proper order.
 *
 * @param addr Unsigned 32 bit integer representing an address value.
 * @param val Unsigned 16 bit integer representing a value to put into the memory.
 *
 ********************************************************************************/

void memory::set16(uint32_t addr, uint16_t val)
{
    uint8_t first = val; 

    set8(addr, first);              // Place the 8 bits in the next address value, 
                                    // since it is little-endian order.

    uint8_t second = (val >> 8);    // Shave off the right-most 8 bits
 
    set8(addr+1, second);             // Place the other 8 bits in the current address value.
}

/**
 * set32(uint32_t addr, uint32_t val) sets values in the "mem" vector.
 *
 * This function uses bit manipulation to prepare the bit values 
 * and calls set16() twice to set the values in the memory in the proper order.
 *
 * @param addr Unsigned 32 bit integer representing an address value.
 * @param val Unsigned 32 bit integer representing a value to put into the memory.
 *
 ********************************************************************************/

void memory::set32(uint32_t addr, uint32_t val)
{
    uint16_t first = val; 

    set16(addr, first);             // Place the 16 bits in the next address value, 
                                      // since it is little-endian order.

    uint16_t second = (val >> 16);    // Shave off the right-most 16 bits

    set16(addr+2, second);              // Place the other 16 bits in the current address value.
}

/**
 * dump() dumps out the contents of the memory in hex as well as the ASCII values.
 *
 * This function prints out the data in the "mem" vector in a formatted fashion.
 *
 ********************************************************************************/

void memory::dump() const
{
    int vectorSize = mem.size();
    std::stringstream strstr;

    for (int i = 0; i < vectorSize; i++)    // Iterate through the entire "mem" vector
    {

        if (i % 8 == 0 && i % 16 != 0)      // Space in between every 8 bytes
        {
            std::cout << ' ';
        }

        if (i % 16 == 0)
        {
            std::cout << to_hex32(i) << ": ";   // Print out hex value of address every new line
        }

        std::cout << to_hex8(mem[i]) << ' ';    // Print out each byte in the memory

        uint8_t ch = get8(i);
        ch = isprint(ch) ? ch : '.';            // ASCII character, or a dot? 
        strstr << ch;

        if ((i+1) % 16 == 0 && i != 0)
        {

            std::cout << '*' << strstr.str() << "*\n"; // Stringstream output of ASCII portion
            strstr.str("");  // Reset string
        }
    }
}

/**
 * load_file(const std::string & fname) opens the file.
 *
 * This function attempts to open the file in binary mode and read its contents
 * into the "mem" vector. It catches an out_of_range exception if the program is too big.
 *
 * @param fname The file to be opened. 
 *
 * @return false if the file cannot be opened or if the program is too big, 
 *         true if there were no errors. 
 *
 ********************************************************************************/

bool memory::load_file(const std::string & fname)
{
    std::ifstream infile(fname, std::ios::in|std::ios::binary);

    if (infile.is_open() == false)
    {
        std::cerr << "Can't open file '" << fname << "' for reading.\n";

        return false;
    }

    uint8_t i;
    infile >> std::noskipws;
    for (uint32_t addr = 0; infile >> i; ++addr)
    {
        try
        {
            mem.at(addr) = i;  // This throws an exception when out of range
        }
        catch (const std::out_of_range& oor)
        {
            std::cout << "WARNING: Address out of range: " << to_hex0x32(i) << std::endl;
            std::cerr << "Program too big.\n";
            infile.close();
            infile.clear();
            return false;
        } 
    }

    infile.close();
    infile.clear();
    return true;
}
