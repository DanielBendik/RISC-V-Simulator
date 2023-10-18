//******************************************************************
//
// Author: Daniel Bendik
// RISC-V Simulator
//
//******************************************************************

#include "hex.h"
#include "memory.h"
#include "rv32i_decode.h"
#include "rv32i_hart.h"
#include "cpu_single_hart.h"
#include "registerfile.h"
#include <iostream>
#include <unistd.h>
#include <vector>


/**
 * disassemble() is a function that takes the memory vector as a parameter. 
 *
 * This function formats the output and calls the decode() function to decode
 * the rv32i instructions (stored in mem) into their most basic representative state. 
 *
 ********************************************************************************/

static void disassemble(const memory &mem)
{
	uint32_t vectorSize = mem.get_size();

    for (uint32_t i = 0; i < vectorSize; i += 4)  // Iterate through memory
    {
		std::cout << hex::to_hex32(i) << ": " << std::setw(8) << std::setfill('0') << std::hex 
				  << mem.get32(i) << "  " << rv32i_decode::decode(i, mem.get32(i)) << std::endl;
	}
}

/**
 * usage() is a validity check.
 *
 * This function tells the user how to pass arguments for this 
 * program and prints to std::cerr in case of an invalid argument.
 * User must specify a file, and can pass memory size as an argument. 
 *
 ********************************************************************************/

static void usage()
{
	std::cerr << "Usage: rv32i [-d] [-i] [-r] [-z] [-l exec-limit] [-m hex-mem-size] infile" << std::endl;
	std::cerr << "    -d show disassembly before program execution" << std::endl;
	std::cerr << "    -i show instruction printing during execution" << std::endl;
	std::cerr << "    -l maximum number of instructions to exec" << std::endl;
	std::cerr << "    -m specify memory size (default = 0x100)" << std::endl;
	std::cerr << "    -r show register printing during execution" << std::endl;
	std::cerr << "    -z show a dump of the regs & memory after simulation" << std::endl;
	exit(1);
}

/**
 * main() displays all the disassembled data information (instructions, registers, addresses)
 * as well as the simulated memory vector.
 *
 * main() sets a default memory size as well, which can be defined in the command line.
 *
 ********************************************************************************/

int main(int argc, char **argv)
{
	uint32_t memory_limit = 0x100; // default memory size = 256 bytes
	int opt;

	bool dFlag = false;
	bool zFlag = false;
	bool iFlag = false;
	bool rFlag = false;

	uint64_t limiter = 0;

	while ((opt = getopt(argc, argv, "dirzm:l:")) != -1)
	{
		switch (opt)
		{
		case 'm':
		{
			std::istringstream iss(optarg);
			iss >> std::hex >> memory_limit;
		}
			break;
		case 'd':
		{
			dFlag = true;
		}
			break;
		case 'i':
		{
			iFlag = true;
		}
			break;
		case 'l':
		{
			limiter = atoi(optarg);
		}
			break;
		case 'r':
		{
			rFlag = true;
		}
			break;
		case 'z':
		{
			zFlag = true;
		}
			break;
		default: /* '?' */
			usage();
		}
	}

	if (optind >= argc)
		usage(); // missing filename

	memory mem(memory_limit);

	if (!mem.load_file(argv[optind]))
		usage();

	cpu_single_hart core(mem);

	if (dFlag == true) 
	{
		disassemble(mem);
		core.reset();
	}

	if (iFlag == true)
	{
		core.reset();  // last thing changed because of 0x00000100 

		if (rFlag == true)
		{
			core.dump("");
			core.set_show_registers(true);
		}
		core.set_show_instructions(true);
	}

	core.run(limiter);

	if (zFlag == true) 
	{ 
		core.dump("");
		mem.dump();
	}

	return 0;
}
