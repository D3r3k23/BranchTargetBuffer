
#ifndef BTB_HPP
#define BTB_HPP

#include "branch_target_buffer.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <array>
#include <cstdint>


const int BTB_SIZE = 1024;
const bool PRINT_INACTIVE_ENTRIES = false;


template <typename SM> // Which prediction state machine to use: Class_SM, SM_B
class BTB
{
private:
    struct Entry
    {
        uint32_t PC = 0;
        uint32_t target = 0;
        SM prediction;
        bool busy;
    };

    std::array<Entry, BTB_SIZE> table;
    Stats stats;
    
public:
    BTB(void);

    static unsigned int address_to_index(uint32_t address);
    
    void add_entry(unsigned int index, uint32_t PC, uint32_t nextPC);

    void process_trace(const std::vector<uint32_t>& trace);
    
    void process_instruction(uint32_t PC, uint32_t nextPC);

    void process_last_instruction(uint32_t PC);

    void print_to_file(const char* fn) const;
};


// Template definitions
#include "BTB.cpp"


#endif // BTB_HPP
