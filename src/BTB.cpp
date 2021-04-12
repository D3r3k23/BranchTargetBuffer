
#include "BTB.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>


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

unsigned int address_to_btb_index(uint32_t address)
{
    return ((address >> 2) & 0x3FF);
}


void BTB::load_from_trace(const Trace& trace)
{
    auto currAddress = trace.traces.begin();
    auto nextAddress = currAddress + 1;

    while (nextAddress != trace.traces.end())
    {
        if (*nextAddress != *currAddress + 4) // Is branch; Next PC == Current PC + 4
        {
            Entry entry = { *currAddress, *nextAddress, PredictionSM(), true };
            unsigned int entryIndex = address_to_btb_index(*currAddress);

            if (entryIndex >= BTB_SIZE)
                std::cout << "Incorrect index calculated, entry skipped." << '\n';
            else
                table[entryIndex] = entry;
        }

        currAddress++;
        nextAddress++;
    }
}

void BTB::print_to_file(const std::string& fn)
{
    std::ofstream oFile(fn, std::ios::out);
    if (oFile.is_open())
    {
        oFile << "Index, PC, TargetPC, Prediction" << '\n';

        for (unsigned int index = 0; const auto& entry : table)
        {
            if (entry.busy)
            {
                oFile << std::setw(4) << index << std::setw(0) << ", ";

                oFile << std::hex << std::uppercase << entry.PC << ", ";
                oFile << entry.targetPC << std::nouppercase << std::dec << '\n'; // ", ";

                // oFile << entry.prediction << '\n';
            }
            index++;
        }
    }
}
