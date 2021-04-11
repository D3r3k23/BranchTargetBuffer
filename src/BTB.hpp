
#ifndef BTB_HPP
#define BTB_HPP

#include <string>
#include <vector>
#include <cstdint>


const int BTB_SIZE = 1024;


class Trace
{
public:
    Trace(const std::string& fn)
        : fn(fn)
    {
        load_from_file();
    }

    int size(void) const
    {
        return traces.size();
    }

    std::vector<uint32_t> get_branches(void);

private:
    void load_from_file(void);

private:
    std::string fn;
    std::vector<uint32_t> traces;
};


int address_to_btb_index(uint32_t address);


enum Prediction
{

};


class PredictionSM
{

};


struct Entry
{
    uint32_t currentPC;
    uint32_t targetPC;
    PredictionSM prediction;
    bool busy;
};


class Stats
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
