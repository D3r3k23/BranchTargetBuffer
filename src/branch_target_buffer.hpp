
#ifndef BRANCH_TARGET_BUFFER_HPP
#define BRANCH_TARGET_BUFFER_HPP

#include <vector>
#include <iostream>
#include <cstdint>


std::vector<uint32_t> load_trace_file(const char* fn);


enum State
{
    S0 = 0b00,
    S1 = 0b01,
    S2 = 0b10,
    S3 = 0b11
};

std::ostream& operator<<(std::ostream& os, State state);


class Class_SM
{
private:
    State state;

public:
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
};

std::ostream& operator<<(std::ostream& os, const Stats& stats);


#endif // BRANCH_TARGET_BUFFER_HPP
