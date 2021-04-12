
#include "BTB.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>


const char* TRACE_FILE = "data/trace_sample.txt";


int main(int, char*[])
{
    auto trace = load_trace_file(TRACE_FILE);
    std::cout << "Traces from: " << TRACE_FILE << " loaded." << '\n';
    std::cout << "Number of traces: " << trace.size() << '\n';

    BTB<Class_SM> btb;
    btb.process_trace(trace);
    btb.print_to_file("output/BTB.txt");

    return 0;
}
