//******************************************************************
//
// Author: Daniel Bendik
// RISC-V Simulator
//
//******************************************************************

#ifndef H_REGISTER_FILE
#define H_REGISTER_FILE

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "hex.h"

class registerfile : public hex
{
public:
    registerfile();
    ~registerfile();

    void reset();
    void set(uint32_t r, int32_t val);
    int32_t get(uint32_t r) const;
    void dump(const std::string &hdr) const;

private:
    std::vector<int32_t> regVec;
};

#endif
