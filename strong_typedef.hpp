#pragma once

#include <array>
#include <boost/hana.hpp>
#include <ratio>
#include <string>
#include <vector>
//#include <boost/hana/experimental/printable.hpp>
#include <cassert>
#include <utility>

#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "si_prefixes.hpp"
//#include <iostream>

//namespace hana = boost::hana;

auto f() {
  auto a = parse_units(si::length<1>{});
  std::cout << a << "\n";
  return a;
}

/*
struct Vector3D {
  float x, y, z;
  Vector3D& operator+=(const Vector3D& other) {
    x += other.x; y += other.y; z += other.z;
    return *this;
  }
};


template<class Tag, class Units, class Ratio = std::ratio<1, 1>, class
BaseType=double> struct Quantity { BaseType _val; template<class OtherRatio>
  Quantity& operator+=(const Quantity<Tag, Units, OtherRatio, BaseType>& other)
{ using Ratio2 = std::ratio_divide<Ratio, OtherRatio>; _val += other._val *
static_cast<double>(Ratio2::num) / Ratio2::den; return *this;
  }
};
*/
/*template<
class m;
class s;
using Length = Units<m>;
using Velocity = Units<m, s<-3>>;

using Position = Quantity<class Pos, Length, Vector3D, std:kilo>;

void update(Position& a, const Velocity& b) {
  a += b;
}
*/
/*
Length<m>;
Density<kg, m<-3>>
Density<tonne, l<-3>>
Stress<MPa>
Stress<Pa, std::mega>
using Newton = Units<kg, m, s<-2>>
using Stress = Units<N, m<-2>>;
//Quantity<N, m<-2>, std::mega>
Stress<kg, m<-1>, s<-2>, std::mega>
Fuel<l, std::kilo, DieselTag, int>
*/ /*
 template<class T, class Ratio>
 struct Quantity {
   T _val;
   template<class T1, class R1>
   Quantity& operator+=(const Quantity<T1, R1>& other) {
     using Ratio2 = std::ratio_divide<Ratio, R1>;
     _val += other._val * double(Ratio2::num) / Ratio2::den;
     return *this;
   }
 };*/
/*
template<class T, class R>
std::ostream& operator<<(std::ostream& os, const Quantity<T, R>& q) {
  os << q._val << "\n";
  return os;
}*/
/*

auto f() {
  //using MPa = std::mega;
  //using GPa = std::giga;
  //using Pa = std::ratio<1,1>;

  //auto modulus = Quantity<double, GPa>{1};
  auto stress = Quantity<double, MPa>{1};
  modulus += stress;
  return modulus;
  //std::cout << modulus;

}



*/
