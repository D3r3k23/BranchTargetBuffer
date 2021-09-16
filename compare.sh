echo "======== Building C++ program ========"
echo ""
rm -f C++/BranchTargetBuffer
build_start="$(date -u +%s.%N)"
make
build_end="$(date -u +%s.%N)"
echo ""

echo "======== Running C++ program ========"
echo ""
cpp_start="$(date -u +%s.%N)"
C++/BranchTargetBuffer
cpp_end="$(date -u +%s.%N)"
echo ""

echo "======== Running Python program ========"
echo ""
python_start="$(date -u +%s.%N)"
python3 Python/BranchTargetBuffer.py
python_end="$(date -u +%s.%N)"
echo ""

build_elapsed=$(echo "$build_end - $build_start" | bc)
cpp_elapsed=$(echo "$cpp_end - $cpp_start" | bc)
python_elapsed=$(echo "$python_end - $python_start" | bc)

echo "Execution time (sec.)"
echo "C++ build:     " $build_elapsed
echo "C++ program:   " $cpp_elapsed
echo "Python program:" $python_elapsed
