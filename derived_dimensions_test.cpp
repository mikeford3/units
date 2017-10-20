#include "derived_dimensions.hpp"
#include <catch.hpp>

SCENARIO("Derived") {
  GIVEN("some basic instantiation checks") {
    auto a = si::derived<si::length<1>>{};
    auto b = si::derived<si::length<-10>>{};
    std::cout << a << "\n" << b << "\n";
    using speed = si::derived<si::length<1>, si::time<-1>>;
    std::cout << si::derived<speed>{};
  }
}