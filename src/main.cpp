
#include "BTB.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <array>


const std::string TRACE_FILE = "data/trace_sample.txt";


int main(int, char*[])
{
    Trace trace(TRACE_FILE);
    std::cout << "Traces from: " << TRACE_FILE << " loaded." << '\n';
    std::cout << "Number of traces: " << trace.size() << '\n';

    std::vector<uint32_t> branches = trace.get_branches();
    std::ofstream oFile("output/branches_list.txt", std::ios::out);
    if (oFile.is_open())
    {
        oFile << "<Branch address>: <BTB index>" << '\n';

        for (const auto& address : branches)
        {
            oFile << std::hex << address << std::dec << ": ";
            oFile << address_to_btb_index(address) << '\n';
        }
    }


    std::array<Entry, BTB_SIZE> btb;

    return 0;
}
