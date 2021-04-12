
#include "BTB.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>


std::vector<uint32_t> load_trace_file(const char* fn)
{
    std::vector<uint32_t> trace;
    std::ifstream iFile(fn, std::ios::in);

    if (iFile.is_open())
    {
        std::string line;
        while (std::getline(iFile, line))
        {
            if (line.size() == 0) // Empty line - end of file reached
                break;

            uint32_t address = std::stoi(line, nullptr, 16);
            trace.push_back(address);
        }
    }
    return trace;
}

unsigned int address_to_btb_index(uint32_t address)
{
    return ((address >> 2) & 0x3FF);
}

void Class_SM::next_state(bool taken)
{
    switch(state)
    {
    case S0 : state = taken ? S0 : S1;
    case S1 : state = taken ? S0 : S2;
    case S2 : state = taken ? S1 : S3;
    case S3 : state = taken ? S2 : S3;
    }
}

void SM_B::next_state(bool taken)
{
    switch(state)
    {
    case S0 : state = taken ? S0 : S1;
    case S1 : state = taken ? S0 : S2;
    case S2 : state = taken ? S0 : S3;
    case S3 : state = taken ? S2 : S3;
    }
}

std::ostream& operator<<(std::ostream& os, const Stats& stats)
{
    os << "Instruction count: " << stats.IC         << '\n';
    os << "Hits:              " << stats.hits       << '\n';
    os << "Misses:            " << stats.misses     << '\n';
    os << "Right:             " << stats.right      << '\n';
    os << "Wrong:             " << stats.wrong      << '\n';
    os << "Taken:             " << stats.taken      << '\n';
    os << "Collisions:        " << stats.collisions << '\n';
    os << "Wrong address:     " << stats.wrong_addr << '\n';
    return os;
}

template <typename SM>
void BTB<SM>::process_trace(const std::vector<uint32_t>& trace)
{
    auto currAddress = trace.begin();
    auto nextAddress = currAddress + 1;

    while (nextAddress != trace.end())
    {
        stats.IC++;

        if (*nextAddress != *currAddress + 4) // Is branch; Next PC == Current PC + 4
        {
            Entry entry = { *currAddress, *nextAddress, SM(), true };
            unsigned int entryIndex = address_to_btb_index(*currAddress);

            if (entryIndex >= BTB_SIZE)
                std::cout << "Incorrect index calculated, entry skipped." << '\n';
            else
            {
                update_entry(entry, entryIndex);
            }
        }
        currAddress++;
        nextAddress++;
    }
    stats.IC++;
}

template <typename SM>
void BTB<SM>::update_entry(Entry<SM> entry, unsigned int index)
{
    auto& targetEntry = table[index];

    if (targetEntry.busy)
    {
        if (entry.PC != targetEntry)
        {
            stats.collisions++;
            stats.misses++;
            targetEntry = entry;
        }
        else
        {
            stats.hits++;
            if (entry.targetPC == targetEntry.targetPC)
            {
                stats.right++;
            }
        }
    }
    else
    {
        stats.misses++;
        targetEntry.busy = true;
    }
}

template <typename SM>
void BTB<SM>::print_to_file(const char* fn) const
{
    std::ofstream oFile(fn, std::ios::out);
    if (oFile.is_open())
    {
        oFile << "Index, PC, TargetPC, " << '\n'; // Prediction" << '\n';

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
