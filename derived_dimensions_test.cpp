#include "common_quantities.hpp"
#include "common_units.hpp"
#include "derived_dimensions.hpp"
#include "derived_dimensions_printing.hpp"
#include <catch.hpp>

SCENARIO("Derived") {
  GIVEN("some basic instantiation checks") {
    auto a = units::derived_t<units::Length<1>>();

    auto b = units::derived_t<units::Length<-10>>();
    std::cout << a << "\n" << b << "\n";
    using speed = units::derived_t<units::Length<1>, units::Time<-1>>;

    std::cout << units::derived_t<speed>() << "\n";
    std::cout << units::derived_t<units::Length<1, 1, units::kilo>>{} *
                     units::derived_t<units::Length<1, 1, units::centi>>{};
    std::cout << "\nPositive roots : \n\t"
              << units::derived_t<units::Length<1, 2>>{} << "\t"
              << units::derived_t<units::Length<1, 3>>{} << "\t"
              << units::derived_t<units::Length<1, 4>>{} << "\t"
              << units::derived_t<units::Length<1, 5>>{} << "\t"
              << units::derived_t<units::Length<1, 6>>{} << "\t"
              << units::derived_t<units::Length<1, 7>>{} << "\n\t"
              << units::derived_t<units::Length<1, 8>>{} << "\t"
              << units::derived_t<units::Length<2, 4>>{} << "\t"
              << units::derived_t<units::Length<2, 5>>{} << "\t"
              << units::derived_t<units::Length<2, 7>>{} << "\n";
    std::cout << "Negative roots : \n\t"
              << units::derived_t<units::Length<-1, 2>>{} << "\t"
              << units::derived_t<units::Length<-1, 3>>{} << "\t"
              << units::derived_t<units::Length<-1, 4>>{} << "\t"
              << units::derived_t<units::Length<-1, 5>>{} << "\t"
              << units::derived_t<units::Length<-1, 6>>{} << "\t"
              << units::derived_t<units::Length<-1, 7>>{} << "\n\t"
              << units::derived_t<units::Length<-1, 8>>{} << "\t"
              << units::derived_t<units::Length<-2, 4>>{} << "\t"
              << units::derived_t<units::Length<-2, 5>>{} << "\t"
              << units::derived_t<units::Length<-2, 7>>{} << "\n";

    std::cout << "Positive ints (and 0): \n\t"
              << units::derived_t<units::Length<0>>{} << "\t"
              << units::derived_t<units::Length<1>>{} << "\t"
              << units::derived_t<units::Length<2>>{} << "\t"
              << units::derived_t<units::Length<3>>{} << "\t"
              << units::derived_t<units::Length<4>>{} << "\t"
              << units::derived_t<units::Length<5>>{} << "\t"
              << units::derived_t<units::Length<6>>{} << "\n\t"
              << units::derived_t<units::Length<7>>{} << "\t"
              << units::derived_t<units::Length<8>>{} << "\t"
              << units::derived_t<units::Length<9>>{} << "\t"
              << units::derived_t<units::Length<10>>{} << "\t"
              << units::derived_t<units::Length<11>>{} << "\t"
              << units::derived_t<units::Length<12>>{} << "\n";

    std::cout << "Negative ints (and 0): \n\t"
              << units::derived_t<units::Length<-0>>{} << "\t"
              << units::derived_t<units::Length<-1>>{} << "\t"
              << units::derived_t<units::Length<-2>>{} << "\t"
              << units::derived_t<units::Length<-3>>{} << "\t"
              << units::derived_t<units::Length<-4>>{} << "\t"
              << units::derived_t<units::Length<-5>>{} << "\t"
              << units::derived_t<units::Length<-6>>{} << "\n\t"
              << units::derived_t<units::Length<-7>>{} << "\t"
              << units::derived_t<units::Length<-8>>{} << "\t"
              << units::derived_t<units::Length<-9>>{} << "\t"
              << units::derived_t<units::Length<-10>>{} << "\t"
              << units::derived_t<units::Length<-11>>{} << "\t"
              << units::derived_t<units::Length<-12>>{} << "\n";

    std::cout
        << units::derived_t<units::Length<2>, units::Time<-1>, units::Mass<1>>{}
        << "\n";
  }

  GIVEN("the same dimensions with different prefixes") {
    auto one = units::derived_t<units::Length<1>>{};
    auto ten = units::derived_t<units::Length<1, 1, std::ratio<10, 1>>>{};
    auto hundred = units::derived_t<units::Length<1, 1, std::ratio<100, 1>>>{};
    auto thousand = units::derived_t<units::Length<1, 1, units::kilo>>{};
    auto inch = units::derived_t<units::Length<1, 1, std::ratio<254, 10000>>>{};

    std::cout << "10^0 " << one << ", 10^1 " << ten << ", 10^2 " << hundred
              << ", 10^3 " << thousand << ", inch " << inch << "\n";
  }

  GIVEN("the same dimensions with different powers") {
    auto a = units::derived_t<units::Length<1>>{};
    auto b = units::derived_t<units::Length<-10>>{};
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
    auto a = units::derived_t<units::Length<1>>{};
    auto b = units::derived_t<units::Mass<-10>>{};
    WHEN("Multiplying them together") {
      auto c = a * b;
      std::cout << a << " * " << b << " = " << c << "\n";
    }
    WHEN("Dividing them together") {
      auto c = a / b;
      std::cout << a << " / " << b << " = " << c << "\n";
    }
  }

  GIVEN("what should be dimensionless units") {
    using dimless0 = units::derived_t<units::unity>;
    using dimless1 = units::derived_t<units::kilo>;
    using dimensioned = units::derived_t<units::Length<1>>;

    static_assert(is_dimensionless(dimless0{}));
    static_assert(is_dimensionless(dimless1{}));
    static_assert(!is_dimensionless(dimensioned{}));
  }

  GIVEN("a couple of units defined in common_units") {
    using kg_per_m3 = units::derived_t<kg_t, units::Length<-3, 1>>;
    THEN("invert should work for a simple case") {
      //auto a = per_second_t{} == invert(seconds_t{});
      static_assert(std::is_same_v<per_second_t, decltype(invert(seconds_t{}))>);
    }

    THEN("then using decltype and / should work") {
      static_assert(std::is_same_v<kg_per_m3, decltype(kg_t{} / m3_t{})>);
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