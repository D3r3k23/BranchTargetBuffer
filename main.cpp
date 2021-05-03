
#include "BTB.hpp"


const std::string TRACE_NAME = "trace_sample";
using SM = Class_SM;


int main(int, char*[])
{
    if (auto trace = load_trace_file(TRACE_NAME + ".txt"); !trace.empty())
    {
        BTB<SM> btb;
        btb.process_trace(trace);
        btb.print_to_file("BTB." + TRACE_NAME + "." + SM::string() + ".csv");
    }
    return 0;
}
