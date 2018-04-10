#include "common_quantities.hpp"
#include "common_units.hpp"
#include "derived_dimensions.hpp"
#include "derived_dimensions_printing.hpp"
#include <catch.hpp>

SCENARIO("Derived") {
  GIVEN("some basic instantiation checks") {
    auto a = si::derived_t<si::Length<1>>();

    auto b = si::derived_t<si::Length<-10>>();
    std::cout << a << "\n" << b << "\n";
    using speed = si::derived_t<si::Length<1>, si::Time<-1>>;

    std::cout << si::derived_t<speed>() << "\n";
    std::cout << si::derived_t<si::Length<1, 1, si::kilo>>{} *
                     si::derived_t<si::Length<1, 1, si::centi>>{};
    std::cout << "\nPositive roots : \n\t" << si::derived_t<si::Length<1, 2>>{}
              << "\t" << si::derived_t<si::Length<1, 3>>{} << "\t"
              << si::derived_t<si::Length<1, 4>>{} << "\t"
              << si::derived_t<si::Length<1, 5>>{} << "\t"
              << si::derived_t<si::Length<1, 6>>{} << "\t"
              << si::derived_t<si::Length<1, 7>>{} << "\n\t"
              << si::derived_t<si::Length<1, 8>>{} << "\t"
              << si::derived_t<si::Length<2, 4>>{} << "\t"
              << si::derived_t<si::Length<2, 5>>{} << "\t"
              << si::derived_t<si::Length<2, 7>>{} << "\n";
    std::cout << "Negative roots : \n\t" << si::derived_t<si::Length<-1, 2>>{}
              << "\t" << si::derived_t<si::Length<-1, 3>>{} << "\t"
              << si::derived_t<si::Length<-1, 4>>{} << "\t"
              << si::derived_t<si::Length<-1, 5>>{} << "\t"
              << si::derived_t<si::Length<-1, 6>>{} << "\t"
              << si::derived_t<si::Length<-1, 7>>{} << "\n\t"
              << si::derived_t<si::Length<-1, 8>>{} << "\t"
              << si::derived_t<si::Length<-2, 4>>{} << "\t"
              << si::derived_t<si::Length<-2, 5>>{} << "\t"
              << si::derived_t<si::Length<-2, 7>>{} << "\n";

    std::cout << "Positive ints (and 0): \n\t" << si::derived_t<si::Length<0>>{}
              << "\t" << si::derived_t<si::Length<1>>{} << "\t"
              << si::derived_t<si::Length<2>>{} << "\t"
              << si::derived_t<si::Length<3>>{} << "\t"
              << si::derived_t<si::Length<4>>{} << "\t"
              << si::derived_t<si::Length<5>>{} << "\t"
              << si::derived_t<si::Length<6>>{} << "\n\t"
              << si::derived_t<si::Length<7>>{} << "\t"
              << si::derived_t<si::Length<8>>{} << "\t"
              << si::derived_t<si::Length<9>>{} << "\t"
              << si::derived_t<si::Length<10>>{} << "\t"
              << si::derived_t<si::Length<11>>{} << "\t"
              << si::derived_t<si::Length<12>>{} << "\n";

    std::cout << "Negative ints (and 0): \n\t"
              << si::derived_t<si::Length<-0>>{} << "\t"
              << si::derived_t<si::Length<-1>>{} << "\t"
              << si::derived_t<si::Length<-2>>{} << "\t"
              << si::derived_t<si::Length<-3>>{} << "\t"
              << si::derived_t<si::Length<-4>>{} << "\t"
              << si::derived_t<si::Length<-5>>{} << "\t"
              << si::derived_t<si::Length<-6>>{} << "\n\t"
              << si::derived_t<si::Length<-7>>{} << "\t"
              << si::derived_t<si::Length<-8>>{} << "\t"
              << si::derived_t<si::Length<-9>>{} << "\t"
              << si::derived_t<si::Length<-10>>{} << "\t"
              << si::derived_t<si::Length<-11>>{} << "\t"
              << si::derived_t<si::Length<-12>>{} << "\n";

    std::cout << si::derived_t<si::Length<2>, si::Time<-1>, si::Mass<1>>{}
              << "\n";
  }

  GIVEN("the same dimensions with different prefixes") {
    auto one = si::derived_t<si::Length<1>>{};
    auto ten = si::derived_t<si::Length<1, 1, std::ratio<10, 1>>>{};
    auto hundred = si::derived_t<si::Length<1, 1, std::ratio<100, 1>>>{};
    auto thousand = si::derived_t<si::Length<1, 1, si::kilo>>{};
    auto inch = si::derived_t<si::Length<1, 1, std::ratio<254, 10000>>>{};

    std::cout << "10^0 " << one << ", 10^1 " << ten << ", 10^2 " << hundred
              << ", 10^3 " << thousand << ", inch " << inch << "\n";
  }

  GIVEN("the same dimensions with different powers") {
    auto a = si::derived_t<si::Length<1>>{};
    auto b = si::derived_t<si::Length<-10>>{};
    WHEN("Multiplying them together") {
      auto c = a * b;
      std::cout << a << " * " << b << " = " << c << "\n";
    }
    WHEN("Dividing them together") {
      auto c = a / b;
      std::cout << a << " / " << b << " = " << c << "\n";
    }
  }

  GIVEN("different dimensions with different powers") {
    auto a = si::derived_t<si::Length<1>>{};
    auto b = si::derived_t<si::Mass<-10>>{};
    WHEN("Multiplying them together") {
      auto c = a * b;
      std::cout << a << " * " << b << " = " << c << "\n";
    }
    WHEN("Dividing them together") {
      auto c = a / b;
      std::cout << a << " / " << b << " = " << c << "\n";
    }
  }
}
/*
struct SomeDimensions {
  Mass<ton> mass;
  Speed speed;                 // m/s, float
  Velocity<mph> velocity;      // miles per hour,
  Derived<std::mega, m, s<-2>> // m/s^2 x 10^6
      using Gravities = Derived<m, s<-2>, std::ratio<981, 100>> // 9.81 m/s^2
      using Speed = Derived<m, s<-1>>;
  using Acceleration = Derived<Speed, s<-1>>;
  using MarsGravities = Derivec < Accleration,
        std::ratio_multiple<std::ratio<376, 1000>, Gravities>;
} */