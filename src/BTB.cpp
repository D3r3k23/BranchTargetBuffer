
#include "BTB.hpp"

#include <filesystem>


template <typename SM>
BTB<SM>::BTB(void)
{
    for (auto& entry : table)
        entry.busy = false;
}

template <typename SM>
unsigned int BTB<SM>::address_to_index(uint32_t address)
{
    return ((address >> 2) & 0x3FF);
}

template <typename SM>
void BTB<SM>::add_entry(unsigned int index, uint32_t PC, uint32_t nextPC)
{
    if (index < BTB_SIZE)
    {
        Entry& entry = table[index];
        
        if (!entry.busy)
        {
            entry.PC = PC;
            entry.target = nextPC;
            entry.prediction.reset();
            entry.busy = true;
        }
    }
}

template <typename SM>
void BTB<SM>::process_trace(const std::vector<uint32_t>& trace)
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

template <typename SM>
void BTB<SM>::process_instruction(uint32_t PC, uint32_t nextPC)
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
            if (taken && (nextPC != entry.target)) // Wrong address
            {
                stats.wrong++;
                stats.wrong_addr++;

                entry.target = nextPC;
            }
            else
                stats.right++;
        }
        else // Wrong prediction
            stats.wrong++;
        
        entry.prediction.go_to_next_state(taken);
    }
    else if (taken) // BTB miss
    {
        stats.misses++;

        if (entry.busy) // Collision
        {
            stats.collisions++;
            entry.busy = false;
        }

        add_entry(index, PC, nextPC);
    }
}

template <typename SM>
void BTB<SM>::process_last_instruction(uint32_t PC)
{
    stats.IC++;

    unsigned int index = address_to_index(PC);
    const Entry& entry = table[index];

    if (entry.busy && (entry.PC == PC))
        stats.hits++;
}

template <typename SM>
void BTB<SM>::print_to_file(const std::string& fn) const
{
    using path = std::filesystem::path;
    if (std::ofstream oFile(path("output") / path(fn), std::ios::out); oFile.is_open())
    {
        oFile << "Index, PC, Target, State Machine, Prediction, Busy" << '\n';
        oFile << std::uppercase << std::boolalpha;

        for (unsigned int index = 0; const auto& entry : table)
        {
            if (entry.busy || PRINT_INACTIVE_ENTRIES)
            {
                oFile << std::dec << std::setw(4) << std::setfill(' ') << index << ", ";

                oFile << std::hex << std::setw(8) << std::setfill('0') << entry.PC << ", ";
                oFile << std::hex << std::setw(8) << std::setfill('0') << entry.target << ", ";

                oFile << entry.prediction.get_state() << ", ";
                oFile << (entry.prediction.taken() ? "Taken" : "NT   ") << ", ";
                oFile << entry.busy;

                oFile << '\n';
            }
            index++;
        }
        std::cout << "BTB contents printed to: " << fn << '\n';
    }
    else
        std::cout << "Could not open: " << fn << '\n';
}
