
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

const bool PRINT_INACTIVE_ENTRIES = true;


std::vector<uint32_t> load_trace_file(const char* fn)
{
    std::vector<uint32_t> trace;
    std::ifstream iFile(fn, std::ios::in);

    if (!iFile.is_open())
        std::cout << "Could not open file: " << fn << '\n';
    else
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
    }
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
    os << std::bitset<2>(static_cast<unsigned int>(state));
    return os;
}


class Class_SM
{
private:
    State state;

public:
    Class_SM()
        { reset(); }

    State reset(void)
        { state = S0; }

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
        { state = S0; }

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

    os << "Hit rate:       " << static_cast<float>(stats.hits)
                             / (static_cast<float>(stats.hits) + static_cast<float>(stats.misses))
                             << "%" << '\n';
    os << "Accuracy:       " << static_cast<float>(stats.right) / static_cast<float>(stats.hits)
                             << "%" << '\n';
    os << "Incorrect Addr: " << static_cast<float>(stats.wrong_addr) / static_cast<float>(stats.wrong)
                             << "%" << '\n';

    return os;
}


template <typename SM>
class BTB
{
private:
    struct Entry
    {
        uint32_t PC;
        uint32_t targetPC;
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
        for (auto address = trace.begin(); (address + 1) != trace.end(); address++)
        {
            process_instruction(*address, *(address + 1));
        }
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

            entry.prediction.go_to_next_state();
        }
        else // BTB miss
        {
            stats.misses++;

            if (taken) // Add to BTB
            {
                if (entry.busy) // Collision
                    stats.collisions++;

                entry.PC = PC;
                entry.targetPC = nextPC;
                entry.prediction.reset();
            }
        }
        std::cout << stats;
    }

    void print_to_file(const char* fn) const
    {
        std::ofstream oFile(fn, std::ios::out);
        if (!oFile.is_open())
            std::cout << "Could not open: " << fn << '\n';
        else
        {
            oFile << "Index, PC, TargetPC, State Machine, Prediction";
            if (PRINT_INACTIVE_ENTRIES)
                oFile << ", Busy";
            oFile << '\n';

            oFile << std::setfill('0') << std::uppercase << std::boolalpha;

            for (unsigned int index = 0; const auto& entry : table)
            {
                if (entry.busy || PRINT_INACTIVE_ENTRIES)
                {
                    oFile << index << ", ";

                    oFile << std::hex << std::setw(8);
                    oFile << entry.PC << ", " << entry.targetPC << ", ";
                    oFile << std::setw(0) << std::dec;

                    oFile << entry.prediction.get_state() << ", ";
                    oFile << (entry.prediction.taken() ? "Taken" : "NT   ") << ", ";
                    oFile << entry.busy;
                }
                index++;
            }
        }
        std::cout << "BTB printed to: " << fn << '\n';
    }

private:
    static unsigned int address_to_index(uint32_t address)
    {
        return ((address >> 2) & 0x3FF);
    }
};


#endif // BTB_HPP
