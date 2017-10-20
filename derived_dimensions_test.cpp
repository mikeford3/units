#include "derived_dimensions.hpp"
#include <catch.hpp>

SCENARIO("Derived") {
  GIVEN("some basic instantiation checks") {
    auto a = si::derived<si::length<1>>{};
    std::cout << a;
  }
}