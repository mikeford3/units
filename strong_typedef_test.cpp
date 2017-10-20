#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "strong_typedef.hpp"

//TEST_CASE("Test Units")
/*
class Anything;
struct Unitless {
  static constexpr int scale = 1;
};

struct Thousand {
  static constexpr int scale = 1;
};

TEST_CASE("Compound") {
  SECTION("Main") {
  //auto fun() {
    auto base1 = Impl::parse_base_unit(Impl::Length<1>{}); //ok
    auto base2 = Impl::parse_arg(Impl::Length<1>{});
    auto a = Impl::parse_units(Impl::Length<1>{}, Impl::Time<-2>{}, Impl::Time<20>{});
    auto b = parse_units();
    //auto c = parse_units(speed{});
   //return base1;
  //}
  }*/
  /*using Velocity = Quantity<metres, Seconds<-1>>;
  using Acceleration = Quantity<Velocity, Seconds<-1>>;
  using EnergyDensity = Quantity<Energy, Length<metre<-2>>;
  using Volume = Quantity<Length<meters<3>>>; //
  using Pressure = Quantity<Energy, Volume<-1>>;
  using Stress = Quantity<Newtons, metres<-2>>
  using FT = Quantity<Stress, metres<std::radio<1, 2>>>;*/
  
//}
/*
TEST_CASE("Strong Typedef") {
  SECTION("Basic Type, no extras") {
    using NoFrills =
        StrongTypedef<Anything, int, Unitless, NotAlgebraic, NotComparable>;
    [[maybe_unused]] auto a = NoFrills{10};
    // REQUIRE(a + a); // error
    // REQUIRE(a += a); error
    // REQUIRE(a <= a); //error
  }
  SECTION("Basic Type, comparable") {
    using Comparable =
        StrongTypedef<Anything, int, Unitless, NotAlgebraic, IsComparable>;
    auto a = Comparable{10};
    auto b = Comparable{11};
    auto c = Comparable{10};

    // REQUIRE(a + a); //error
    // REQUIRE(a += a); //error
    REQUIRE(a <= a);
    REQUIRE(a <= b);
    REQUIRE(a <= c);
    REQUIRE(a == a);
    REQUIRE(a == c);
    REQUIRE(a != b);
    REQUIRE(a < b);

    REQUIRE(b != a);
    REQUIRE(b > a);
    REQUIRE(b >= a);
  }

  SECTION("Basic Type, algebraic") {
    using Algebraic =
        StrongTypedef<Anything, int, Unitless, IsAlgebraic, NotComparable>;
    auto a = Algebraic{10};
    // REQUIRE(a + a == Algebraic{20}); //error
    REQUIRE((a + a).underlying_value() == 20);

    a += a;
    REQUIRE(a.underlying_value() == 20);
    // REQUIRE(a <= a); //error
  }

  SECTION("Basic Type, algebraic and comparable") {
    using Both =
        StrongTypedef<Anything, int, Unitless, IsAlgebraic, IsComparable>;
    auto a = Both{10};
    REQUIRE(a.underlying_value() == 10);
    REQUIRE(a == Both{10});
    REQUIRE(a + a == Both{20});
    a += a;
    REQUIRE(a == Both{20});
  }

  SECTION("Basic Type, algebraic and comparable, divide by scalar") {
    using Both =
        StrongTypedef<Anything, int, Unitless, IsAlgebraic, IsComparable>;
    auto a = Both{10};
    REQUIRE(a / 5 == Both{2});
    REQUIRE(a / 2 == Both{5});
    REQUIRE(a / 3 == Both{3});
  }

  SECTION("Basic Type, algebraic and comparable, multiply by scalar") {
    using Both =
        StrongTypedef<Anything, int, Unitless, IsAlgebraic, IsComparable>;
    auto a = Both{10};
    REQUIRE(a * 5 == Both{50});
    REQUIRE(a * 2 == Both{20});
    REQUIRE(a * 3 == Both{30});
    REQUIRE(5 * a == Both{50});
    REQUIRE(2 * a == Both{20});
    REQUIRE(3 * a == Both{30});
  }

  SECTION("Basic Type, algebraic and comparable, multiply by scalar") {
    using Both =
        StrongTypedef<Anything, int, Unitless, IsAlgebraic, IsComparable>;
    auto a = Both{10};
    a *= 5;
    REQUIRE(a == Both{50});
  }

  SECTION("Divide StrongTypedef by a time to get a rate") {
    using Both =
        StrongTypedef<Anything, double, Unitless, IsAlgebraic, IsComparable>;
    auto a = Both{10};
    auto rate = a / Seconds{1};
    REQUIRE(Rate<Both>{10} == rate);
  }
}
TEST_CASE("StrongTypedef - check cannot convert") {
  class One_t;
  class Two_t;
  using One = StrongTypedef<One_t, int, Unitless, IsAlgebraic, IsComparable>;
  using Two = StrongTypedef<Two_t, int, Unitless, IsAlgebraic, IsComparable>;
  [[maybe_unused]] auto one = One{10};
  [[maybe_unused]] auto two = Two{10};
  SECTION("Copy etc, should error") {
    // REQUIRE(one == two);
    // REQUIRE(one > two);
    // REQUIRE(one(two) == one);
  }
}

TEST_CASE("Rates with different values") {
  using Position1 = StrongTypedef<Anything, double, Unitless, IsAlgebraic, IsComparable>;
  using Position1000 = StrongTypedef<Anything, double, Thousand, IsAlgebraic, IsComparable>;
  SECTION("TESTS") {
    auto rate1_1 = Rate<Position1, Seconds>{1};
  }
  
  
}

TEST_CASE("Rates of Rates of Rates") {
  using Position =
      StrongTypedef<class Pos, double, Unitless, IsAlgebraic, IsComparable>;
  using Velocity = Rate<Position>;
  using Acceleration = Rate<Velocity>;
  const auto p0 = Position{2};
  const auto p1 = Position{6};
  const auto v0 = Velocity{1};
  const auto v1 = Velocity{3};
  const auto acc0 = Acceleration{0};
  const auto acc1 = Acceleration{4};
  SECTION("Test velocity from positions") {
    REQUIRE(v0 == (Position{4} / Seconds{4}));
    REQUIRE(v1 == (Position{12} / Seconds{4}));
  }
  SECTION("Test acceleration from velocity") {
    REQUIRE(acc0 == (Velocity{0} / Seconds{4}));
    REQUIRE(acc1 == (Velocity{16} / Seconds{4}));
  }
  SECTION("Velocity from acceleration") { REQUIRE(v0 == acc1 * Seconds{0.25}); }
  SECTION("Position from velocity") { REQUIRE(p0 == v0 * Seconds{2}); }
  SECTION("Euler") { REQUIRE(p1 == p0 + v0 * Seconds{4}); }
  SECTION("Euler middle") {
    // p1 = p0 + (v0 + v1) / 2, v1 = v0 + acc * Sec
    //      ?  == 2 + (1 + (1 + 16)) / 2) == 2 + 9
    REQUIRE(Position{11} == p0 + (v0 + (v0 + acc1 * Seconds{4})) / 2);
  }
}

TEST_CASE("Rate") {
  class AnythingOther;
  using Resource =
      StrongTypedef<AnythingOther, double, Unitless, IsAlgebraic, IsComparable>;
  auto rate = Rate<Resource>{Resource{10}};

  SECTION("Rate constructor") {
    auto r1 = Rate<Resource>{Resource{10}};
    auto r2 = Rate<Resource>{10};
    REQUIRE(r1 == r2);
  }

  SECTION("Rate test multiply by time return resource") {
    REQUIRE(Resource{10} == rate * Seconds{1});
    REQUIRE(Resource{20} == rate * Seconds{2});
  }

  SECTION("Rate test multiply by int") {
    REQUIRE(Rate<Resource>{Resource{20}} == rate * 2);
  }

  SECTION("Rate test multiply by float") {
    REQUIRE(Rate<Resource>{Resource{20}} == rate * 2.f);
  }

  SECTION("Rate test multiply by double") {
    REQUIRE(Rate<Resource>{Resource{20}} == rate * 2.0);
  }

  SECTION("Rate test *= by double") {
    rate *= 2.;
    REQUIRE(Rate<Resource>{Resource{20}} == rate);
  }

  SECTION("Rate test divide by int") {
    REQUIRE(Rate<Resource>{Resource{5}} == rate / 2);
  }

  SECTION("Rate test divide by int with remainder") {
    REQUIRE(Rate<Resource>{Resource{2}} == rate / 5);
  }

  SECTION("Rate test divide by float") {
    REQUIRE(Rate<Resource>{Resource{5}} == rate / 2.f);
  }

  SECTION("Rate test divide by double") {
    REQUIRE(Rate<Resource>{Resource{5}} == rate / 2.0);
  }

  SECTION("Rate with a double test divide by double") {
    using Resource_d = StrongTypedef<AnythingOther, double, Unitless,
                                     IsAlgebraic, IsComparable>;
    auto rate_d = Rate<Resource_d>{Resource_d{10.5}};
    REQUIRE(Rate<Resource_d>{Resource_d{5.25}} == rate_d / 2.0);
  }
}

TEST_CASE("Rescaling things") {
  using base = StrongTypedef<class pwr, float, Unity>;
  using kilo = StrongTypedef<class pwr, float, Kilo>;
  using nano = StrongTypedef<class pwr, float, Nano>;
  SECTION("Rescale direct same") {
    const auto both = rescale(base{1}, base{1});
    auto a = std::get<0>(both);
    auto b = std::get<1>(both);
    REQUIRE(decltype(a)::Scale::scale == 1);
    REQUIRE(decltype(b)::Scale::scale == 1);
  }

  SECTION("Rescale direct different 1") {
    const auto both = rescale(base{1}, kilo{1});
    auto a = std::get<0>(both);
    auto b = std::get<1>(both);
    REQUIRE(decltype(a)::Scale::scale == 1);
    REQUIRE(decltype(b)::Scale::scale == 1);
  }

  SECTION("Rescale direct different 1 ") {
    const auto both = rescale(kilo{1}, base{1});
    auto a = std::get<0>(both);
    auto b = std::get<1>(both);
    REQUIRE(decltype(a)::Scale::scale == 1);
    REQUIRE(decltype(b)::Scale::scale == 1);
  }

  SECTION("Rescale direct both different ") {
    const auto both = rescale(kilo{1}, nano{1});
    auto a = std::get<0>(both);
    auto b = std::get<1>(both);
    REQUIRE(decltype(a)::Scale::scale == 1000);
    REQUIRE(decltype(b)::Scale::scale == 1000);
  }
  SECTION("Equality") {
    REQUIRE(base{1} == base{1});
    REQUIRE(base{1} == kilo{1. / 1000});
    REQUIRE(base(1) == nano{1e9f});
    REQUIRE(kilo{1. / 1000} == base{1});
    REQUIRE(nano{1e9f} == base(1));
    REQUIRE(nano{1e6f} == kilo{1e-6f});
  }

  SECTION("Less than") {
    REQUIRE(base{0.9f} < base{1});
    REQUIRE(base{0.9f} < kilo{1. / 1000});
    REQUIRE(base(0.9f) < nano{1e9f});
  }
  SECTION("Constructors") {
    auto k = kilo{1};
    auto a = base{k};

    auto n = nano{1};
    auto b = base{n};
    REQUIRE(a.val == 1000);
    REQUIRE(b.val == 1e-9f);

    a = b;
    REQUIRE(a.val == 1e-9f);
    

    
  }

}
*/