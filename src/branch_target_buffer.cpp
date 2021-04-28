
#include "branch_target_buffer.hpp"

#include <string>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <filesystem>


std::vector<uint32_t> load_trace_file(const std::string& fn)
{
    std::vector<uint32_t> trace;

    using path = std::filesystem::path;
    if (std::ifstream iFile(path("data") / path(fn), std::ios::in); iFile.is_open())
    {
        std::string line;
        while (std::getline(iFile, line))
        {
            if (line.size() == 0) // Empty line - end of file reached
                break;

            uint32_t address = std::stoi(line, nullptr, 16); // Address is in hex
            trace.push_back(address);
        }
        std::cout << "Traces from: " << fn << " loaded." << '\n';
        std::cout << "Number of traces: " << trace.size() << '\n';
        std::cout << '\n';
    }
    else
        std::cout << "Could not open file: " << fn << '\n' << '\n';

    return trace;
}

std::ostream& operator<<(std::ostream& os, State state)
{
    return os << std::bitset<2>(static_cast<unsigned int>(state));
}

std::ostream& operator<<(std::ostream& os, const Stats& stats)
{
    os << "IC:         " << stats.IC         << '\n';
    os << "Hits:       " << stats.hits       << '\n';
    os << "Misses:     " << stats.misses     << '\n';
    os << "Right:      " << stats.right      << '\n';
    os << "Wrong:      " << stats.wrong      << '\n';
    os << "Taken:      " << stats.taken      << '\n';
    os << "Collisions: " << stats.collisions << '\n';
    os << "Wrong addr: " << stats.wrong_addr << '\n';
    os << '\n';
    os << std::fixed << std::setprecision(2);
    os << "Hit rate:       " << 100.f * (static_cast<float>(stats.hits)
                             / (static_cast<float>(stats.hits) + static_cast<float>(stats.misses)))
                             << "%" << '\n';
    os << "Accuracy:       " << 100.f * (static_cast<float>(stats.right) / static_cast<float>(stats.hits))
                             << "%" << '\n';
    os << "Incorrect addr: " << 100.f * (static_cast<float>(stats.wrong_addr) / static_cast<float>(stats.wrong))
                             << "%" << '\n';
    return os;
}
