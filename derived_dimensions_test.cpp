#include "derived_dimensions.hpp"
#include <catch.hpp>

SCENARIO("Derived") {
  GIVEN("some basic instantiation checks") {
    auto a = si::derived<si::length<1>>{};
    auto b = si::derived<si::length<-10>>{};
    std::cout << a << "\n" << b << "\n";
    using speed = si::derived<si::length<1>, si::time<-1>>;
    std::cout << si::derived<speed>{};
    std::cout << "\n";
    std::cout << si::derived<si::length<1, 2>>{}
              << "\n"; // << si::derived<si::length<-1, 2>>{};
    std::cout << "i = 0, " << si::derived<si::length<0>>{} << "\n";
    std::cout << "i = 1, " << si::derived<si::length<1>>{} << "\n";
    std::cout << "i = 2, " << si::derived<si::length<2>>{} << "\n";
    std::cout << "i = 3, " << si::derived<si::length<3>>{} << "\n";
    std::cout << "i = 4, " << si::derived<si::length<4>>{} << "\n";
    std::cout << "i = 5, " << si::derived<si::length<5>>{} << "\n";
    std::cout << "i = 6, " << si::derived<si::length<6>>{} << "\n";
    std::cout << "i = 7, " << si::derived<si::length<7>>{} << "\n";
    std::cout << "i = 8, " << si::derived<si::length<8>>{} << "\n";
    std::cout << "i = 9, " << si::derived<si::length<9>>{} << "\n";
    std::cout << "i = 10, " << si::derived<si::length<10>>{} << "\n";
    std::cout << "i = 11, " << si::derived<si::length<11>>{} << "\n";
    std::cout << "i = 12, " << si::derived<si::length<12>>{} << "\n";
    std::cout << "i = 13, " << si::derived<si::length<13>>{} << "\n";
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