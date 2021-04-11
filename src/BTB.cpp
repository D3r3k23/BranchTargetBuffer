
#include "BTB.hpp"

#include <iostream>
#include <fstream>


void Trace::load_from_file(void)
{
    std::ifstream iFile(fn, std::ios::in);
    if (iFile.is_open())
    {
        std::string line;
        while (std::getline(iFile, line))
        {
            if (line.size() == 0) // Empty line - end of file reached
                break;

            uint32_t address = std::stoi(line, nullptr, 16);
            traces.push_back(address);
        }
    }
}

std::vector<uint32_t> Trace::get_branches(void)
{
    std::vector<uint32_t> branches;

    for (auto it = traces.begin(); it != (traces.end() - 1); it++)
        if (*(it + 1) != *it + 4) // Next PC == Current PC + 4
            branches.push_back(*it);
        
    return branches;
}

int address_to_btb_index(uint32_t address)
{
    return ((address >> 2) & 0x3FF);
}
