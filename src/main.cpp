
#include "branch_target_buffer.hpp"
#include "BTB.hpp"

#include <string>


const std::string trace_name = "trace_sample";
using SM = Class_SM;


int main(int, char*[])
{
    if (auto trace = load_trace_file(trace_name + ".txt"); !trace.empty())
    {
        BTB<SM> btb;
        btb.process_trace(trace);
        btb.print_to_file("BTB." + trace_name + "." + SM::string() + ".csv");
    }
    return 0;
}
