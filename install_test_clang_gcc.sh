#!/bin/bash


files=(string_constants.hpp prefixes.hpp base_dimensions.hpp derived_dimensions.hpp derived_dimensions_printing.hpp derived_dimensions_impl.hpp quantity.hpp numeric_functions.hpp common_units.hpp common_quantities.hpp units.hpp)
#copy header files into system header
for fname in ${files[*]}; do
    cp $fname "/usr/local/include/"$fname
done

#helper functions to build and run tests in gcc and clang
make_gcc() {
    g++ unity_test.cpp -o Unity_Test_GCC.exe -std=c++2a -Wall -Wpedantic
    echo "gcc build complete"
}
test_gcc() {
    ./Unity_Test_GCC.exe ~[profile]
}
make_clang() {
    clang++ unity_test.cpp -o Unity_Test_clang.exe -std=c++2a -Wall -Wpedantic
    echo "clang build complete"
}
test_clang() {
    ./Unity_Test_clang.exe ~[profile]
}

#run build and tests for gcc and clang in seperate processes
(
    make_gcc
    test_gcc
) &
echo "building with gcc...."
(
    make_clang
    test_clang
) &
echo "building with clang...."

wait < <(jobs -p)
