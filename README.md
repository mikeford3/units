A library to allow the use of arbitrary physical units in C++ code to allow compile time checks (dimensional analysis) and better naming without runtime overhead.

# Problem
To calculate kinetic energy from velocity and mass $(\frac{1}{2} mv^2)$ you could write a function using doubles, like one of the following:

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
* the conversion from tons to kg is wrong. Easy enough to spot here, but what conversion factor is needed to get pressure in MPa from square inches and pounds?

The problem is that the compiler sees a function that takes some doubles, multiples them together and returns a double, and therefore generates perfectly valid code.

# Examples
## Simple example
Using specialisations of the units::Quantity class to define joules, kg, metres_per_second etc:
```C++
#include <units.hpp>
// joules   in [kg m^2 s^-2]
// kg       in [kg]
// velocity in [m/s]
joules kinetic_energy(kg mass, metres_per_second velocity) {
    joules energy = 0.5 * mass * velocity;          // 1
    return energy;
}
// and call it
ton mass = 1.5 
metres_per_second velocity = 40 
joules energy = kinetic_energy(velocity, mass)      // 2, 3
```
The compiler will error at:
1. mass * velocity gives momentum $[kgms^{-1}]$ which cannot be assigned to energy $[kgm^2s^{-2}]$ 
2. velocity is being passed as an argument to the mass parameters, and mass to velocity, which cannot be assigned to each other. 
3. The conversion of mass from tons to kg is handled by the compiler.

But the correct code below will compile:
```C++
joules kinetic_energy(kg mass, metres_per_second velocity) {
    joules energy = 0.5 * mass * velocity * velocity;   
    return energy;
}
// and call it
ton mass = 1.5 
metres_per_second velocity = 40 
joules energy = kinetic_energy(mass, velocity)
```
## Library types
### Quantity
The joules, ton, metres_per_second types used above are specialisation of the Quantity class in the units library.  The Quantity class is a bit like a strong typedef containing:
* the dimensions, such as Length and Mass, and a prefix (or scaling parameter) to distinguish between tons, kg, grams, wrapped in a derived_t class,
* an underlying data type, defaults to double, 
* an optional tag to distinguish between quantities with the same units that the user wants to keep separate.
```C++
template<class Units, class UnderlyingType = double, class Tag = std::false_type>
class Quantity{...}
// where Units is an instance of the derived_t
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
These correspond to the 7 base dimension in the SI system, listed above, and the numerator and denominator of their exponent, and the prefix.
```C++
template <intmax_t n = 1, intmax_t d = 1, class prefix = std::ratio<1,1>>
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
using kg = Quantity<kg_t> // types with an _t are derived_t types
using ton = Quantity<ton_t>; 
using metres_per_second = Quantity<metres_per_sec_t>;
using joule = Quanity<joule_t>;

// Which use the following derived_t types
using kg_t = derived_t<Mass<1>>;
using ton_t = derived_t<kg_t, std::ratio<1000, 1>>;      
using metres_t = derived_t<Length<1>>; 
using metres_per_sec_t = derived_t<metres_t, Time<-1>>;
using joule_t = derived_t<Mass, Length<2>, Time<-2>>;

// Although the Quantities can just be defined without the _t typedefs, e.g.
using ton = Quantity<derived_t<Mass<1>, std::ratio<1000,1>>>; 
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

# Printing values
The `<<` operator is overloaded for Quanties, which prints out both the value and the units (the derived_t), for example the derived_t values could be (using the unicode symbols for superscript 1, 2, 3 etc and for the roots):
Positive roots : 

        √m      ∛m      ∜m      m⁰⋅²⁰   m⁰⋅¹⁶   m⁰⋅¹⁴
        m⁰⋅¹²   √m      m⁰⋅⁴⁰   m⁰⋅²⁸
Negative roots : 

        m⁻⁰⋅⁵⁰  m⁻⁰⋅³³  m⁻⁰⋅²⁵  m⁻⁰⋅²⁰  m⁻⁰⋅¹⁶  m⁻⁰⋅¹⁴
        m⁻⁰⋅¹²  m⁻⁰⋅⁵⁰  m⁻⁰⋅⁴⁰  m⁻⁰⋅²⁸
Positive ints (and 0): 

                m       m²      m³      m⁴      m⁵      m⁶
        m⁷      m⁸      m⁹      m¹⁰     m¹¹     m¹²
Negative ints (and 0): 

                m⁻¹     m⁻²     m⁻³     m⁻⁴     m⁻⁵     m⁻⁶
        m⁻⁷     m⁻⁸     m⁻⁹     m⁻¹⁰    m⁻¹¹    m⁻¹²
A more complicated unit:

        m²kgs⁻¹

Printing out a Quantity of 1 million metres ^ -0.5 kg s^-2:

        1 Mm⁻⁰⋅⁵⁰kgs⁻²  

# Compile times
Are bad (and a work in progress!)
I think defining the commonly used types in a .cpp file would allow the compiler to instantiate them once only. The `using ...` approach outlined above is simpler, but causes the compiler to instantiate types in every translation unit they are used in. 

# Dependencies







