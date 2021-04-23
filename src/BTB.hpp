
#ifndef BTB_HPP
#define BTB_HPP

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <cstdint>


const int BTB_SIZE = 1024;
const bool PRINT_INACTIVE_ENTRIES = false;


std::vector<uint32_t> load_trace_file(const char* fn)
{
    std::vector<uint32_t> trace;

    if (std::ifstream iFile(fn, std::ios::in); iFile.is_open())
    {
        std::string line;
        while (std::getline(iFile, line))
        {
            if (line.size() == 0) // Empty line - end of file reached
                break;

            uint32_t address = std::stoi(line, nullptr, 16); // Address is in hex
            trace.push_back(address);
        }
        std::cout << "Traces from: " << fn << " loaded." << '\n';
        std::cout << "Number of traces: " << trace.size() << '\n';
        std::cout << '\n';
    }
    else
        std::cout << "Could not open file: " << fn << '\n' << '\n';

    return trace;
}


enum State
{
    S0 = 0b00,
    S1 = 0b01,
    S2 = 0b10,
    S3 = 0b11
};

std::ostream& operator<<(std::ostream& os, State state)
{
    return os << std::bitset<2>(static_cast<unsigned int>(state));
}


class Class_SM
{
private:
    State state;

public:
    Class_SM()
        { reset(); }

    State reset(void)
        { return (state = S0); }

    bool taken(void) const
        { return (state == S0) || (state == S1); }

    State go_to_next_state(bool taken)
    {
        switch(state)
        {
        case S0: state = taken ? S0 : S1;  break;
        case S1: state = taken ? S0 : S2;  break;
        case S2: state = taken ? S1 : S3;  break;
        case S3: state = taken ? S2 : S3;  break;
        }
        return state;
    }

    State get_state(void) const
        { return state; }
};

class SM_B
{
private:
    State state;

public:
    SM_B()
        { reset(); }

    State reset(void)
        { return (state = S0); }

    bool taken(void) const
        { return (state == S0) || (state == S1); }

    State go_to_next_state(bool taken)
    {
        switch(state)
        {
        case S0: state = taken ? S0 : S1;  break;
        case S1: state = taken ? S0 : S2;  break;
        case S2: state = taken ? S0 : S3;  break;
        case S3: state = taken ? S2 : S3;  break;
        }
        return state;
    }

    State get_state(void) const
        { return state; }
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

    // friend std::ostream& operator<<(std::ostream& os, const Stats& stats);
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
    os << '\n';
    os << "Hit rate:       " << static_cast<float>(stats.hits)
                             / (static_cast<float>(stats.hits) + static_cast<float>(stats.misses))
                             << "%" << '\n';
    os << "Accuracy:       " << static_cast<float>(stats.right) / static_cast<float>(stats.hits)
                             << "%" << '\n';
    os << "Incorrect addr: " << static_cast<float>(stats.wrong_addr) / static_cast<float>(stats.wrong)
                             << "%" << '\n';
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
        bool busy;
    };

    std::array<Entry, BTB_SIZE> table;
    Stats stats;
    
public:
    BTB()
    {
        for (auto& entry : table)
        {
            entry.busy = false;
        }
    }

    void process_trace(const std::vector<uint32_t>& trace)
    {
        auto address = trace.begin();
        while (address != trace.end() - 1)
        {
            process_instruction(*address, *(address + 1));
            address++;
        }
        process_last_instruction(*address);

        std::cout << "Stats:" << '\n' << '\n';
        std::cout << stats;
        std::cout << '\n';
    }

    void process_instruction(uint32_t PC, uint32_t nextPC)
    {
        stats.IC++;

        unsigned int index = address_to_index(PC);
        Entry& entry = table[index];

        bool taken = (nextPC != PC + 4);
        if (taken)
            stats.taken++;

        if (entry.busy && (entry.PC == PC)) // BTB hit
        {
            stats.hits++;

            if (entry.prediction.taken() == taken) // Right prediction
            {
                if (taken && (nextPC != entry.targetPC)) // Wrong address
                {
                    stats.wrong++;
                    stats.wrong_addr++;

                    entry.targetPC = nextPC;
                }
                else
                {
                    stats.right++;
                }
            }
            else // Wrong prediction
            {
                stats.wrong++;
            }
            entry.prediction.go_to_next_state(taken);
        }
        else if (taken) // BTB miss
        {
            stats.misses++;

            if (entry.busy) // Collision
                stats.collisions++;

            entry = { PC, nextPC, SM(), true }; // Add to BTB
        }
    }

    void process_last_instruction(uint32_t PC)
    {
        stats.IC++;

        unsigned int index = address_to_index(PC);
        const Entry& entry = table[index];

        if (entry.busy && (entry.PC == PC))
            stats.hits++;
    }

    void print_to_file(const char* fn) const
    {
        if (std::ofstream oFile(fn, std::ios::out); oFile.is_open())
        {
            oFile << "Index, PC, TargetPC, State Machine, Prediction, Busy" << '\n';

            oFile << std::uppercase << std::boolalpha;

            for (unsigned int index = 0; const auto& entry : table)
            {
                if (entry.busy || PRINT_INACTIVE_ENTRIES)
                {
                    oFile << std::setw(4) << index << ", ";

                    oFile << std::hex << std::setw(8) << std::setfill('0') << entry.PC << ", ";
                    oFile << std::hex << std::setw(8) << std::setfill('0') << entry.targetPC << ", ";

                    oFile << std::dec << std::setw(0) << std::setfill(' ');

                    oFile << entry.prediction.get_state() << ", ";
                    oFile << (entry.prediction.taken() ? "Taken" : "NT   ") << ", ";
                    oFile << entry.busy;

                    oFile << '\n';
                }
                index++;
            }
            oFile << std::nouppercase << std::noboolalpha;

            std::cout << "BTB contents printed to: " << fn << '\n';
        }
        else
            std::cout << "Could not open: " << fn << '\n';
    }

private:
    static unsigned int address_to_index(uint32_t address)
    {
        return ((address >> 2) & 0x3FF);
    }
};


#endif // BTB_HPP
