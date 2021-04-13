
#ifndef BTB_HPP
#define BTB_HPP

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <array>
#include <cstdint>


const int BTB_SIZE = 1024;


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


enum State : uint8_t
{
    S0 = 0b00,
    S1 = 0b01,
    S2 = 0b10,
    S3 = 0b11
};

class Class_SM
{
public:
    Class_SM() : state(S0) { }

    bool taken(void) const
        { return state == S0 || state == S1; }

    void next_state(bool taken)
    {
        switch(state)
        {
        case S0 : state = taken ? S0 : S1; break;
        case S1 : state = taken ? S0 : S2; break;
        case S2 : state = taken ? S1 : S3; break;
        case S3 : state = taken ? S2 : S3; break;
        }
    }

private:
    State state;
};

class SM_B
{
public:
    SM_B() : state(S1) { }

    bool taken(void) const
        { return state == S0 || state == S1; }

    void next_state(bool taken)
    {
        switch(state)
        {
        case S0 : state = taken ? S0 : S1; break;
        case S1 : state = taken ? S0 : S2; break;
        case S2 : state = taken ? S0 : S3; break;
        case S3 : state = taken ? S2 : S3; break;
        }
    }

private:
    State state;
};


struct Stats
{
    int IC = 0;
    int hits = 0;
    int misses = 0;
    int right = 0;
    int wrong = 0;
    int taken = 0;
    int collisions = 0;
    int wrong_addr = 0;

    friend std::ostream& operator<<(std::ostream& os, const Stats& stats);
};

std::ostream& operator<<(std::ostream& os, const Stats& stats)
{
    os << "IC:         " << stats.IC         << '\n';
    os << "Hits:       " << stats.hits       << '\n';
    os << "Misses:     " << stats.misses     << '\n';
    os << "Right:      " << stats.right      << '\n';
    os << "Wrong:      " << stats.wrong      << '\n';
    os << "Taken:      " << stats.taken      << '\n';
    os << "Collisions: " << stats.collisions << '\n';
    os << "Wrong addr: " << stats.wrong_addr << '\n';
    return os;
}


template <typename SM>
class BTB
{
private:
    struct Entry
    {
        uint32_t PC = 0;
        uint32_t targetPC = 0;
        SM prediction;
        bool busy = false;
    };

    std::array<Entry, BTB_SIZE> table;
    Stats stats;
    
public:
    void process_trace(const std::vector<uint32_t>& trace)
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
                    table[entryIndex] = entry;
                    // update_entry(entry, entryIndex);
                }
            }
            currAddress++;
            nextAddress++;
        }
        stats.IC++;
    }

    void print_to_file(const char* fn) const
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

    void print_stats(void) const
        { std::cout << stats; }

private:
    void update_entry(Entry entry, unsigned int index)
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
};


#endif // BTB_HPP
