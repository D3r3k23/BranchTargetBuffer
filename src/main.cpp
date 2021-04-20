
#include "BTB.hpp"


const char* TRACE_FILE = "data/trace_sample.txt";
using SM = Class_SM;


int main(int, char*[])
{
    auto trace = load_trace_file(TRACE_FILE);
    
    BTB<SM> btb;
    btb.process_trace(trace);
    btb.print_to_file("output/BTB.csv");

    return 0;
}
