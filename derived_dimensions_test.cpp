#include "derived_dimensions.hpp"
#include "derived_dimensions_printing.hpp"
#include <catch.hpp>

SCENARIO("Derived") {
  GIVEN("some basic instantiation checks") {
    auto a = si::derived<si::length<1>>{};
    auto b = si::derived<si::length<-10>>{};
    std::cout << a << "\n" << b << "\n";
    using speed = si::derived<si::length<1>, si::time<-1>>;
    std::cout << si::derived<speed>{};
    std::cout << "\nPositive roots : \n\t" << si::derived<si::length<1, 2>>{}
              << "\t" << si::derived<si::length<1, 3>>{} << "\t"
              << si::derived<si::length<1, 4>>{} << "\t"
              << si::derived<si::length<1, 5>>{} << "\t"
              << si::derived<si::length<1, 6>>{} << "\t"
              << si::derived<si::length<1, 7>>{} << "\n\t"
              << si::derived<si::length<1, 8>>{} << "\t"
              << si::derived<si::length<2, 4>>{} << "\t"
              << si::derived<si::length<2, 5>>{} << "\t"
              << si::derived<si::length<2, 7>>{} << "\n";

    std::cout << "Negative roots : \n\t" << si::derived<si::length<-1, 2>>{}
              << "\t" << si::derived<si::length<-1, 3>>{} << "\t"
              << si::derived<si::length<-1, 4>>{} << "\t"
              << si::derived<si::length<-1, 5>>{} << "\t"
              << si::derived<si::length<-1, 6>>{} << "\t"
              << si::derived<si::length<-1, 7>>{} << "\n\t"
              << si::derived<si::length<-1, 8>>{} << "\t"
              << si::derived<si::length<-2, 4>>{} << "\t"
              << si::derived<si::length<-2, 5>>{} << "\t"
              << si::derived<si::length<-2, 7>>{} << "\n";

    std::cout << "Positive ints (and 0): \n\t" << si::derived<si::length<0>>{}
              << "\t" << si::derived<si::length<1>>{} << "\t"
              << si::derived<si::length<2>>{} << "\t"
              << si::derived<si::length<3>>{} << "\t"
              << si::derived<si::length<4>>{} << "\t"
              << si::derived<si::length<5>>{} << "\t"
              << si::derived<si::length<6>>{} << "\n\t"
              << si::derived<si::length<7>>{} << "\t"
              << si::derived<si::length<8>>{} << "\t"
              << si::derived<si::length<9>>{} << "\t"
              << si::derived<si::length<10>>{} << "\t"
              << si::derived<si::length<11>>{} << "\t"
              << si::derived<si::length<12>>{} << "\n";

    std::cout << "Negative ints (and 0): \n\t" << si::derived<si::length<-0>>{}
              << "\t" << si::derived<si::length<-1>>{} << "\t"
              << si::derived<si::length<-2>>{} << "\t"
              << si::derived<si::length<-3>>{} << "\t"
              << si::derived<si::length<-4>>{} << "\t"
              << si::derived<si::length<-5>>{} << "\t"
              << si::derived<si::length<-6>>{} << "\n\t"
              << si::derived<si::length<-7>>{} << "\t"
              << si::derived<si::length<-8>>{} << "\t"
              << si::derived<si::length<-9>>{} << "\t"
              << si::derived<si::length<-10>>{} << "\t"
              << si::derived<si::length<-11>>{} << "\t"
              << si::derived<si::length<-12>>{} << "\n";

    std::cout << si::derived<si::length<2>, si::time<-1>, si::mass<1>>{}
              << "\n";
  }
}
/*
struct SomeDimensions {
  Mass<ton> mass;
  Speed speed;                        // m/s, float
  Velocity<mph> velocity;             // miles per hour,
  Derived<std::mega, m, s<-2>>        // m/s^2 x 10^6
  using Gravities = Derived<m, s<-2>, std::ratio<981, 100>> // 9.81 m/s^2
  using Speed = Derived<m, s<-1>>;
  using Acceleration = Derived<Speed, s<-1>>;
  using MarsGravities = Derivec<Accleration,
std::ratio_multiple<std::ratio<376, 1000>, Gravities>;
}*/