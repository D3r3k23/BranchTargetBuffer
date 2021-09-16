echo "======== Running C++ program (O0) ========"
echo ""
rm -f C++/BranchTargetBuffer
g++ -std=c++2a -O0 -o C++/BranchTargetBuffer C++/main.cpp
cpp_start_O0="$(date -u +%s.%N)"
C++/BranchTargetBuffer
cpp_end_O0="$(date -u +%s.%N)"
echo ""

echo "======== Running C++ program (O1) ========"
echo ""
rm -f C++/BranchTargetBuffer
g++ -std=c++2a -O1 -o C++/BranchTargetBuffer C++/main.cpp
cpp_start_O1="$(date -u +%s.%N)"
C++/BranchTargetBuffer
cpp_end_O1="$(date -u +%s.%N)"
echo ""

echo "======== Running C++ program (O2) ========"
echo ""
rm -f C++/BranchTargetBuffer
g++ -std=c++2a -O2 -o C++/BranchTargetBuffer C++/main.cpp
cpp_start_O2="$(date -u +%s.%N)"
C++/BranchTargetBuffer
cpp_end_O2="$(date -u +%s.%N)"
echo ""

echo "======== Running C++ program (O3) ========"
echo ""
rm -f C++/BranchTargetBuffer
g++ -std=c++2a -O3 -o C++/BranchTargetBuffer C++/main.cpp
cpp_start_O3="$(date -u +%s.%N)"
C++/BranchTargetBuffer
cpp_end_O3="$(date -u +%s.%N)"
echo ""

echo "======== Running Python program ========"
echo ""
python_start="$(date -u +%s.%N)"
python3 Python/BranchTargetBuffer.py
python_end="$(date -u +%s.%N)"
echo ""

cpp_elapsed_O0=$(echo "$cpp_end_O0 - $cpp_start_O0" | bc)
cpp_elapsed_O1=$(echo "$cpp_end_O1 - $cpp_start_O1" | bc)
cpp_elapsed_O2=$(echo "$cpp_end_O2 - $cpp_start_O2" | bc)
cpp_elapsed_O3=$(echo "$cpp_end_O3 - $cpp_start_O3" | bc)
python_elapsed=$(echo "$python_end - $python_start" | bc)

echo "Execution time (sec.)"
echo "C++ (O0): "$cpp_elapsed_O0
echo "C++ (O1): "$cpp_elapsed_O1
echo "C++ (O2): "$cpp_elapsed_O2
echo "C++ (O3): "$cpp_elapsed_O3
echo "Python:   "$python_elapsed
