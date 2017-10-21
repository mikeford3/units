#include "derived_dimensions.hpp"
#include <catch.hpp>

SCENARIO("Derived") {
  GIVEN("some basic instantiation checks") {
    auto a = si::derived<si::length<1>>{};
    auto b = si::derived<si::length<-10>>{};
    std::cout << a << "\n" << b << "\n";
    using speed = si::derived<si::length<1>, si::time<-1,2>>;
    std::cout << si::derived<speed>{};

    std::cout << "\n";
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
  using MarsGravities = Derivec<Accleration, std::ratio_multiple<std::ratio<376, 1000>, Gravities>;
}*/