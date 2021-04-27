
all: src/main.cpp src/branch_target_buffer.cpp
	g++ -g -Wall -std=c++2a -o BranchTargetBuffer src/main.cpp src/branch_target_buffer.cpp
