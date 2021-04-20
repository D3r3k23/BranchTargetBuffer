
#include "BTB.hpp"

#include <iostream>


const char* TRACE_FILE = "data/trace_sample.txt";


int main(int, char*[])
{
    auto trace = load_trace_file(TRACE_FILE);
    
    BTB<Class_SM> btb;
    btb.process_trace(trace);
    btb.print_to_file("output/BTB.csv");

    return 0;
}
