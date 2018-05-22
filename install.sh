#!/bin/sh

cp quantity.hpp base_dimensions.hpp common_quantities.hpp common_units.hpp \
   derived_dimensions.hpp derived_dimensions_printing.hpp prefixes.hpp /usr/local/include

cat prefixes.hpp base_dimensions.hpp derived_dimensions.hpp common_units.hpp quantity.hpp > combined.hpp

for fname in quantity.hpp base_dimensions.hpp common_quantities.hpp common_units.hpp derived_dimensions.hpp derived_dimensions_printing.hpp prefixes.hpp
do
   perl -p -i -e "s/#include \"$fname\"//gc" combined.hpp
done