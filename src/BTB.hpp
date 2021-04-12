
#ifndef BTB_HPP
#define BTB_HPP

#include <iostream>
#include <vector>
#include <array>
#include <cstdint>


const int BTB_SIZE = 1024;


std::vector<uint32_t> load_trace_file(const char* fn);

unsigned int address_to_btb_index(uint32_t address);


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

    void next_state(bool taken);

private:
    State state;
};

class SM_B
{
public:
    SM_B() : state(S1) { }

    bool taken(void) const
        { return state == S0 || state == S1; }

    void next_state(bool taken);

private:
    State state;
};


template <typename SM>
struct Entry
{
    uint32_t PC = 0;
    uint32_t targetPC = 0;
    SM prediction;
    bool busy = false;
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


template <typename SM>
class BTB
{
public:
    void process_trace(const std::vector<uint32_t>& trace);

    void print_to_file(const char* fn) const;

    void print_stats(void) const
        { std::cout << stats; }

private:
    void update_entry(Entry<SM> entry, unsigned int index);

private:
    std::array<Entry<SM>, BTB_SIZE> table;
    Stats stats;
};


#endif // BTB_HPP
