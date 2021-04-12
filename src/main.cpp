
#include "BTB.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>


const std::string TRACE_FILE = "data/trace_sample.txt";


int main(int, char*[])
{
    Trace trace(TRACE_FILE);
    std::cout << "Traces from: " << TRACE_FILE << " loaded." << '\n';
    std::cout << "Number of traces: " << trace.size() << '\n';


    BTB btb(trace);

    btb.print_to_file("output/BTB.txt");


    return 0;
}
