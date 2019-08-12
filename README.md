A library to allow the use of arbitrary physical units in C++ code to allow compile time checks (dimensional analysis) and better naming without runtime overhead.

# Problem
To calculate kinetic energy from velocity and mass (KE = 0.5mv^2) you could write a function using doubles, using comments for the units:

```C++
// Takes mass in kg, velocity in m/s and returns energy in joules
double kinetic_energy(double mass, double velocity) {
    double energy = 0.5 * mass * velocity;
    return energy;
}

double /*energy */ kinetic_energy(double /*kg*/ mass, double /*m/s*/ velocity) {
    double energy = 0.5 * mass * velocity;
    return energy;
}

// Or perhaps using typedefs gives the nicest version
using kg = double;
using metres_per_second  = double;
using joules = double;
joules kinetic_energy(kg mass, metres_per_second velocity) {
    joules energy = 0.5 * mass * velocity * velocity; //  
    return energy;
}

// which can be called as below
double mass = 1.5 // tons
double velocity = 40 // metres per second
double energy = kinetic_energy(velocity, mass / 100) // multiply by 1000 to convert to kg
```
but... 
* the units of the energy calculated in the function are wrong (forgot to square the velocity), but this can't be detected by the compiler,
* the units of the arguments and return value aren't checked, they are in the wrong order,
* the conversion from tons to kg is wrong. Easy enough to spot here, but what conversion factor is needed to get pressure in MPa from psi?

The problem is that the compiler sees a function that takes some doubles, multiples them together and returns a double, and therefore generates perfectly valid code. What we want is to create a wrapper around the doubles, similar to a strong typedef, but more flexible as it must generate the correct type  when operations are applied to two different types (such as multiplication). The important properties are:
* Dimensional analysis checking - error if an incorrect operation is requested, preferable at compile time.
* No runtime overhead - this should be useful for engineering and scientific applications which must run as fast as possible to solve large problems.
* Understandable errors.


# Examples
## Simple example
Using the units::Quantity class to define joules, kg, metres_per_second etc:
```C++
#include <units.hpp>
#include <common_quantities.hpp>
// joules   in [kg m^2 s^-2]
// kg       in [kg]
// velocity in [m/s]
joules kinetic_energy(kg mass, metres_per_second velocity) {
    joules energy = 0.5 * mass * velocity;          // 1
    return energy;
}
// and call it
ton mass{1.5}
metres_per_second velocity{40}
joules energy = kinetic_energy(velocity, mass)      // 2, 3
```
The compiler will error at:
1. mass * velocity gives momentum [kgm/s] which cannot be assigned to energy [kgm^2/s^2]
2. velocity is being passed as an argument to the mass parameters, and mass to velocity, which cannot be assigned to each other. 
3. The conversion of mass from tons to kg is handled by the compiler.

But the correct code below will compile:
```C++
joules kinetic_energy(kg mass, metres_per_second velocity) {
    joules energy = 0.5 * mass * velocity * velocity;   
    return energy;
}
// and call it
ton mass{1.5} 
metres_per_second velocity{40} 
joules energy = kinetic_energy(mass, velocity)
```
## Error messages
The purpose of this library is to produce compiler errors, so a simple example (extracted from the test suite which uses Catch2):
```C++
    THEN("Check compiler error") { REQUIRE(inch{2} == kg{1}); }
```
is (in GCC):
```
In file included from /home/mike/Dropbox_UoM/Coding/units/common_quantities.hpp:6,
                 from /home/mike/Dropbox_UoM/Coding/units/common_quantities_test.cpp:1:
/home/mike/Dropbox_UoM/Coding/units/quantity.hpp: In instantiation of ‘constexpr auto use_dimension_names() [with bool DimensionsBalance = false; lhs = Names<units::Length<1, 1>, units::Mass<0, 1>, units::Time<0, 1>, units::Current<0, 1>, units::Temperature<0, 1>, units::Amount<0, 1>, units::Luminosity<0, 1> >; rhs = Names<units::Length<0, 1>, units::Mass<1, 1>, units::Time<0, 1>, units::Current<0, 1>, units::Temperature<0, 1>, units::Amount<0, 1>, units::Luminosity<0, 1> >]’:
/home/mike/Dropbox_UoM/Coding/units/quantity.hpp:234:69:   required from ‘constexpr auto operator==(const Quantity<Units0, BaseType, Tag>&, const Quantity<Units1, BaseType, Tag1>&) [with Units0 = units::Dimensions<std::ratio<1, 1>, std::ratio<0, 1>, std::ratio<0, 1>, std::ratio<0, 1>, std::ratio<0, 1>, std::ratio<0, 1>, std::ratio<0, 1>, std::ratio<254, 10000> >; Units1 = units::Dimensions<std::ratio<0, 1>, std::ratio<1, 1>, std::ratio<0, 1>, std::ratio<0, 1>, std::ratio<0, 1>, std::ratio<0, 1>, std::ratio<0, 1>, std::ratio<1, 1> >; BaseType = double; Tag0 = std::integral_constant<bool, false>; Tag1 = std::integral_constant<bool, false>]’
/home/mike/Dropbox_UoM/Coding/units/common_quantities_test.cpp:20:36:   required from here
/home/mike/Dropbox_UoM/Coding/units/quantity.hpp:225:17: error: static assertion failed
  225 |   static_assert(DimensionsBalance);
```
From the bottom moving up:
*  The static_assert(DimensionsBalance) is where the static assert fired.
*  common_quanities_test.cpp:20 is where the REQUIRE(inch{2} == kg{1}) was written (the bug).
*  quantity.hpp:234 the call to the equality operator, showing the types of the Quantities being compared - the first 7 std::ratios of each Quantity are the powers of the 7 fundamental types, the last std::ratio is the prefix.
*  details of the static_assert - the "use_dimension_names" is just a way of converting the std::ratios used in the derived_t/Dimensions class to the underlying types with names like Length. Here the units on the left hand side (lhs) are Length<1>, or m, and the rhs are Mass<1>, or kg. 
## Library types
### Quantity
The joules, ton, metres_per_second types used above are specialisation of the Quantity class in the units library.  The Quantity class is a bit like a strong typedef containing:
* the dimensions, such as Length and Mass, and a prefix (or scaling parameter) to distinguish between tons, kg, grams, etc. These are wrapped in a derived_t class,
* an underlying data type, defaults to double, 
* an optional tag to distinguish between quantities with the same units that the user wants to keep separate, defaults to std::false_type.
```C++
template<class Units, class UnderlyingType = double, class Tag = std::false_type>
class Quantity{
    // where Units is an instance of the derived_t
    UnderlyingType value;
    ... constructors & operators
};
```
### derived_t
The derived_t containes the dimensions and prefix the Quantity uses. It accepts variadic template arguments which can be: 
* base dimensions (Length, Mass, Time, Current, Temperature, Amount, or Luminosity), including their dimensional exponent,
* a std::ratio as a prefix, defaults to std::ratio<1,1>,
* another derived_t.
```C++
template<class ...Args>
class derived_t{...}

```
### Base units
These correspond to the 7 base dimension in the SI system, listed above, and the numerator and denominator of their exponent.
```C++
template <intmax_t n = 1, intmax_t d = 1>
struct Length{...}

template <intmax_t n = 1, intmax_t d = 1>
struct Mass{...}

template <intmax_t n = 1, intmax_t d = 1>
struct Time{...}

template <intmax_t n = 1, intmax_t d = 1>
struct Current{...}

template <intmax_t n = 1, intmax_t d = 1>
struct Temperature{...}

template <intmax_t n = 1, intmax_t d = 1>
struct Amount{...}

template <intmax_t n = 1, intmax_t d = 1>
struct Luminosity{...}
```
### Defining types
For example, the Quantities used above could be defined as follows (written top down for ease of reading)
```C++
// Quantities
using kg = Quantity<kg_t> // types with a _t are derived_t types
using ton = Quantity<ton_t>; 
using metres_per_second = Quantity<metres_per_sec_t>;
using joule = Quanity<joule_t>;

// Which use the following derived_t types
using kg_t = derived_t<Mass<1>>;
using ton_t = derived_t<kg_t, std::ratio<1000, 1>>;      
using metres_t = derived_t<Length<1>>; 
using inch_t = derived_t<Length<1>, std::ratio<254, 10000>>;
using metres_per_sec_t = derived_t<metres_t, Time<-1>>;
using joule_t = derived_t<Mass, Length<2>, Time<-2>>;

// The most convenient way to create derived types is with decltype (once a few derived_t's are defined)
using kg_metres_t = decltype(kg_t{} * metres_t{}); // torque
using metres2_t = decltype(metres_t{} * metres_t{}); // area
using ton_per_metres3_t = decltype(ton_t{} / (metres_t{} * metres2_t{})); // density in tons per cubic metre

// Although the Quantities can just be defined without the _t typedefs, e.g.
using ton = Quantity<derived_t<Mass<1>, std::ratio<1000,1>>>; 
using ton_per_cubic_inch = Quanity<decltype(ton_t{} / (inch_t{} * inch_t{} * inch_t{}))>;
```
### Converting types
The derived_t types have constexpr division and multiplication functions that create the correct type without requiring the types to be explicitly declared or defined. For example:
```C++
constexpr auto kinetic_energy(kg mass, metres_per_second velocity) {
    return mass * std::pow<2>(velocity) / 2;
}
```
Will return a Quantity with a derived_t in joules. If the arguments passed to kinetic_energy have the right dimension but different prefixes then this will be handled by the compiler, e.g.
```C++
auto energy = kinetic_energy(ton{25}, inches_per_year{2000});
```
Will still work. The prefixes (std::ratios) would be combined to calculate the correct factor to convert to joules.

# Helper functions
## Printing values
The `<<` operator is overloaded for Quanties, which prints out both the value and the units (the derived_t), for example the derived_t values could be (using the unicode symbols for superscript 1, 2, 3 etc and for the roots):
Positive roots : 

        √m      ∛m      ∜m      m⁰⋅²⁰   m⁰⋅¹⁶   m⁰⋅¹⁴
        m⁰⋅¹²   √m      m⁰⋅⁴⁰   m⁰⋅²⁸
Negative roots : 

        m⁻⁰⋅⁵⁰  m⁻⁰⋅³³  m⁻⁰⋅²⁵  m⁻⁰⋅²⁰  m⁻⁰⋅¹⁶  m⁻⁰⋅¹⁴
        m⁻⁰⋅¹²  m⁻⁰⋅⁵⁰  m⁻⁰⋅⁴⁰  m⁻⁰⋅²⁸
Positive ints (and 0, so the first "missing" value is dimensionless): 

                m       m²      m³      m⁴      m⁵      m⁶
        m⁷      m⁸      m⁹      m¹⁰     m¹¹     m¹²
Negative ints (and 0, so the first "missing" value is dimensionless): 

                m⁻¹     m⁻²     m⁻³     m⁻⁴     m⁻⁵     m⁻⁶
        m⁻⁷     m⁻⁸     m⁻⁹     m⁻¹⁰    m⁻¹¹    m⁻¹²
A more complicated unit:

        m²kgs⁻¹

Printing out a Quantity of 1 million metres ^ -0.5 kg s^-2:

        1 Mm⁻⁰⋅⁵⁰kgs⁻²  

## Comparators
The usual comparison operators are provided: ==, !=, <, <=, >, =>, which account for the prefix provided:
```C++ 
assert(tons{1} > kg{999});
assert(tons{1} < kg{1001});
```
Comparisons between Quantities and other algebraic types (float, double etc) are defined for Quantities with dimensionless derived_ts, which commonly occur with ratios:
```C++
assert((tons{1.0} / kg{1.0}) > 0.9999); // fine, mass / mass is dimensionless so can be compared to a double 
assert((tons(1.0) / metres3{1}) > 0.9999); // error, mass / volume gives a density, kg / m^3, so cannot be compared to a double
```

## Numeric

### Pow
An implementation of pow is provided which returns the correct type, for example:
```C++
assert(pow<2>(metres{2}) == metres2{4});
```
The power to be raised by is passed by template argument as a parameter.

### Square root
An implementation of sqrt is provided which returns the correct type, for example:
```C++
assert(sqrt(metres2{100}) == metres{10});
```

### abs and fabs
```C++
assert(metres{100} == abs(-metres{100}));
assert(metres{100} == fabs(-metres{100}));
```

### Numeric Limits
The following functions and definitions in std::numeric_limits are overloaded\defined for Quantities. In all cases they use the existing definition in std::numeric_limits for the underlying type (double, float etc):
```C++
std::min;
std::max;
std::lowest;
std::epsilon;
std::round_error;
std::infinity;
std::quiet_NaN;
std::signaling_NaN;
std::denorm_min;

std::numeric_limits::max_exponent;
std::numeric_limits::max_exponent10;
std::numeric_limits::min_exponent;
std::numeric_limits::min_exponent10;
std::numeric_limits::radix;
std::numeric_limits::digits;
std::numeric_limits::max_digits10;
std::numeric_limits::is_specialized;
std::numeric_limits::is_signed;
std::numeric_limits::is_integer;
std::numeric_limits::is_exact;
std::numeric_limits::has_infinity;
std::numeric_limits::has_quiet_NaN;
std::numeric_limits::has_signaling_NaN;
std::numeric_limits::has_denorm;
std::numeric_limits::has_denorm_loss;
std::numeric_limits::round_style;
std::numeric_limits::is_iec559;
std::numeric_limits::is_bounded;
std::numeric_limits::is_modulo;
std::numeric_limits::traps;
std::numeric_limits::tinyness_before;
```

### Convenience Functions
A few nice functions are borrowed from Fortran that alias numeric_limits functions:
```C++
huge(Quantity) // calls std::numeric_limits<Quantity>::max; 
tiny(Quantity) // calls std::numeric_limits<Quantity>::min; 
epsilon(Quantity) // calls std::numeric_limits<Quantity>::epsilon; 
```

# Dependencies
* Catch - unit testing library, available on github.
* StringConstant - compile time strings (to be removed now that constexpr strings and vectors are in C++20). The header file is included in this repo, originally copied from https://gist.github.com/dsanders11/8951887. Some minor changes to add the out stream operator (<<) and to silence unused variable warnings (which would stop StringConstant being C++14 so not suggested back to the author).
* boost/hana - metaprogramming library.

# Built and tested with
* GCC 9.1.0 on Ubuntu 19.04, C++2a flag
* Clang 8.0.0-3 on Ubuntu 19.04 C++2a flag

# Weaknesses
## Compile Time
Are bad.
I think defining the commonly used types in a .cpp file would allow the compiler to instantiate them once only. The `using ...` approach outlined above is simpler, but causes the compiler to instantiate types in every translation unit they are used in. 

## Debug Runtime
Are slower than using doubles by a rough factor of 3 in the test cases uses to exercise the code. The Quanity classes are simple for the optimiser to see through - all operations use the single member variable only and the class is only the size of the underlying value (typically double), so release builds are usually as fast as the use of doubles, generating the same code (viewed a number of times in Godbolt).

## How about C++98/03/11/14 or GCC 4,5,6...?
Units hasn't been tested with older compilers or standards. It wasn't written with compatibility as a goal, and Boost Hana requires a very recent compiler in any event.










