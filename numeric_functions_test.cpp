#include "common_quantities.hpp"
#include "numeric_functions.hpp"
#include "quantity.hpp"
#include <catch.hpp>

SCENARIO("Test testing numeric functions") {
  GIVEN("R values")
  WHEN("raising metres to a power") {
    THEN(" 2m * 2m = (2m)^2") {
      REQUIRE(pow<2>(metres{2}) == metres{2} * metres{2});
    }
    THEN(" 2m * 2m * 2m= (2m)^3") {
      REQUIRE(pow<3>(metres{2}) == metres{2} * metres{2} * metres{2});
    }
    THEN(" 2m * 2m * 2m= (2m)^3") {
      REQUIRE(pow<3>(metres{2}) == metres{2} * metres{2} * metres{2});
    }
    THEN(" 1 / 2m = (2m)^-1") { REQUIRE(pow<-1>(metres{2}) == 1 / metres{2}); }
    THEN(" 1 / (2m * 2m) = (2m)^-2") {
      REQUIRE(pow<-2>(metres{2}) == 1 / (metres{2} * metres{2}));
    }
  }

  GIVEN("some common length quantities") {
    auto x0 = metres{1};
    auto x1 = metres{-11};
    WHEN("using abs") {
      REQUIRE(std::abs(x0) == x0);
      REQUIRE(std::abs(x1) == metres{11});
    }
    WHEN("using pow") {
      REQUIRE(pow<2>(x0) == metres2{1});
      REQUIRE(pow<2>(metres{2}) == metres2{4});
      REQUIRE(pow<2>(metres{20}) == metres2{400});
      REQUIRE(pow<2>(metres{-2}) == metres2{4});
      REQUIRE(pow<3>(metres{-2}) == metres3{-8});
      REQUIRE(pow<1>(metres{-2}) == metres{-2});
    }
    WHEN("using sqrt") {
      REQUIRE(std::sqrt(metres{1}) == metres05{1});
      REQUIRE(std::sqrt(metres{9}) == metres05{3});
      REQUIRE(std::sqrt(metres2{9}) == metres{3});
      // REQUIRE(sqrt(m2{-9}) == metres{3});         should assert false
    }
  }
}

template <class BaseType>
void runner() {
  using Quant = Quantity<metres_t, BaseType>;
  WHEN("calling numeric_limits functions that return a Quantity type (such "
       "as max, min)") {
    WHEN("taking the maximum of the Quantity and its underlying type") {
      auto value = std::numeric_limits<Quant>::max();
      auto expected = std::numeric_limits<BaseType>::max();
      THEN("these limits should be equal") {
        REQUIRE(value.underlying_value() == expected);
      }
    }
    WHEN("taking the epsilon of the Quantity and its underlying type") {
      auto value = std::numeric_limits<Quant>::epsilon();
      auto expected = std::numeric_limits<BaseType>::epsilon();
      THEN("these limits should be equal") {
        REQUIRE(value.underlying_value() == expected);
      }
    }

    WHEN("taking the min of the Quantity and its underlying type") {
      auto value = std::numeric_limits<Quant>::min();
      auto expected = std::numeric_limits<BaseType>::min();
      THEN("these limits should be equal") {
        REQUIRE(value.underlying_value() == expected);
      }
    }
  }
  WHEN("testing numeric_limits variables that are primitive (such "
       "as signed or max_exponent)") {
    WHEN("taking the max_exponent of the Quantity and its underlying type") {
      auto value = std::numeric_limits<Quant>::max_exponent;
      auto expected = std::numeric_limits<BaseType>::max_exponent;
      THEN("these limits should be equal") { REQUIRE(value == expected); }
    }

    WHEN("comparing the signedness of the Quantity and its underlying type") {
      auto value = std::numeric_limits<Quant>::is_signed;
      auto expected = std::numeric_limits<BaseType>::is_signed;
      THEN("these limits should be equal") { REQUIRE(value == expected); }
    }

    WHEN("using the huge, tiny and epsilon convenience functions") {
      THEN("huge should match maximum") {
        REQUIRE(huge(Quant{}) == std::numeric_limits<Quant>::max());
      }
      THEN("tiny should match min") {
        REQUIRE(tiny(Quant{}) == std::numeric_limits<Quant>::min());
      }
      THEN("epsilon should match epsilon") {
        REQUIRE(epsilon(Quant{}) == std::numeric_limits<Quant>::epsilon());
      }
    }
  }
}

SCENARIO("Testing std overloads in numeric_limits for Quantities") {
  GIVEN("an underlying type of double") { runner<double>(); }
  GIVEN("an underlying type of float") { runner<float>(); }
  GIVEN("an underlying type of bool") { runner<bool>(); }
  GIVEN("an underlying type of char") { runner<char>(); }
  GIVEN("an underlying type of unsigned char") { runner<unsigned char>(); }
  GIVEN("an underlying type of signed char") { runner<signed char>(); }
  GIVEN("an underlying type of wchar_t") { runner<wchar_t>(); }
  GIVEN("an underlying type of char32_t") { runner<char32_t>(); }
  GIVEN("an underlying type of short") { runner<short>(); }
  GIVEN("an underlying type of long double") { runner<long double>(); }
  GIVEN("an underlying type of long long") { runner<long long>(); }
}