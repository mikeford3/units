#!/bin/bash

rm -f ~units0.hpp units.hpp 

#files to combine, order important (quantity depends on derived_dimensions...)
files=(prefixes.hpp base_dimensions.hpp derived_dimensions_impl.hpp \
    derived_dimensions_printing.hpp common_units.hpp quantity.hpp \
    common_quantities.hpp numeric_functions.hpp derived_dimensions.hpp)


    
#combine them all into a temporary file, add an empty line as a break (some files
#might not end with a newline)
for fname in ${files[*]}
do
    cat $fname >> ~units0.hpp
    echo "" >> ~units0.hpp    
done

#strip out includes to local files in the library
for fname in ${files[*]}
do
   perl -p -i -e "s/#include \"$fname\"//gc" ~units0.hpp
done

#strip out all the "#pragma once"
perl -p -i -e "s/#pragma once//gc" ~units0.hpp

#create list of system includes, <vector> etc
#  change delimiter for array from whitespace to end line (otherwise array
#  would split #include <vector> into two elements 
IFS_backup=$IFS
IFS=$'\n'
#  create array
global_includes=( `grep "#include <" \~units0.hpp` )

unique_gi=( `for i in ${global_includes[@]}; do echo $i; done | sort -u` )

#strip out includes to system includes in the file
for fname in ${unique_gi[*]}
do
   perl -p -i -e "s/${fname//'/'/'\/'}//gc" ~units0.hpp
done


#put in unique system includes at the top, and pragma once
echo "//Do not edit, generated file!" > units.hpp
echo "#pragma once" >> units.hpp
for fname in ${unique_gi[*]}
do
    echo $fname >> units.hpp
done

#copy in rest of file
cat ~units0.hpp >> units.hpp
rm -f ~units0.hpp


#copy into system header
cp units.hpp /usr/local/include/units.hpp


#create a single test file to run suite units.hpp
echo "#include <units.hpp>" > unity_test.cpp
for fname in *_test.cpp
do 
    if [ "$fname" != "unity_test.cpp" ]
    then
        cat $fname >> unity_test.cpp
        echo "" >> unity_test.cpp
    fi
done

#strip out local includes
for fname in ${files[*]}
do  
    if [ "$fname" != "unity_test.cpp" ]
    then   
        inc_string="#include \"$fname\""
        perl -p -i -e "s/${inc_string}//gc" unity_test.cpp

    fi
done

#helper functions to build and run tests in gcc and clang
make_gcc(){
    g++ unity_test.cpp -o Unity_Test_GCC.exe -std=c++2a -Wall -Wpedantic
    echo "gcc build complete"
}
test_gcc(){
    ./Unity_Test_GCC.exe ~[profile]
}
make_clang(){
    clang++ unity_test.cpp -o Unity_Test_clang.exe -std=c++2a -Wall -Wpedantic
    echo "clang build complete"
}
test_clang(){
    ./Unity_Test_clang.exe ~[profile]
}

#run build and tests for gcc and clang in seperate processes
( make_gcc; test_gcc )  &
echo "building with gcc...."
( make_clang; test_clang ) &
echo "building with clang...."

wait < <(jobs -p)

cp units.hpp /usr/local/include/units.hpp

#cp files for individual includes
for fname in ${files[*]}
do
    cp $fname "/usr/local/include/"$fname
done



#reset delimiter
IFS=IFS_backup