
#ifndef BTB_HPP
#define BTB_HPP

#include <string>
#include <vector>
#include <array>
#include <cstdint>


const int BTB_SIZE = 1024;


struct Trace
{
    std::string fn;
    std::vector<uint32_t> traces;

    Trace(const std::string& fn)
        : fn(fn)
    {
        load_from_file();
    }

    int size(void) const
    {
        return traces.size();
    }

    void load_from_file(void);
};


unsigned int address_to_btb_index(uint32_t address);


enum Prediction
{

};


class PredictionSM
{

};


struct Entry
{
    uint32_t PC = 0;
    uint32_t targetPC = 0;
    PredictionSM prediction;
    bool busy = false;
};


class BTB
{
public:
    BTB(const Trace& trace)
    {
        load_from_trace(trace);
    }

    void load_from_trace(const Trace& trace);
    void print_to_file(const std::string& fn);

private:
    void add_new_entry(Entry entry);
    void update_entry(Entry entry);

private:
    std::array<Entry, BTB_SIZE> table;
};


struct Stats
{
    int IC;
    int hits;
    int misses;
    int right;
    int wrong;
    int taken;
    int collisions;
    int wrong_addr;
};


#endif // BTB_HPP
