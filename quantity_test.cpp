#include "quantity.hpp"
#include <catch.hpp>
#include <iostream>

SCENARIO("Create some quantities of physical units") {
  using metre_l = decltype(si::derived<si::L<1>>());
  GIVEN("a quantity with only a unit type, default otherwise") {
    const double dval = 10.;
    auto quant = Quantity<metre_l>{dval};
    REQUIRE(quant.underlying_value() == dval);
    WHEN("multiply a quanity by itself") {
      auto quant_sq = quant * quant;
      THEN("the units must have changed") {
        // std::cout << quant;
        std::cout << quant_sq;
        static_assert(
            std::is_same_v<Quantity<decltype(si::derived<si::Length<2>>())>,
                           decltype(quant_sq)>);
      }
    }
    WHEN("operating with floating points") {
      WHEN("dividing") {
        auto divided = quant / 1.5;
        THEN(" it should be different") {
          REQUIRE(divided.underlying_value() == dval / 1.5);
        }
      }
      WHEN("divide assigning ") {
        quant /= 1.5;
        THEN(" it should be different") {
          REQUIRE(quant.underlying_value() == dval / 1.5);
        }
      }
      WHEN("multiplying") {
        auto mult1 = quant * 1.5;
        auto mult2 = 1.5 * quant;
        THEN(" it should be different (one order)") {
          REQUIRE(mult1.underlying_value() == dval * 1.5);
        }
        THEN(" it should be different (other order)") {
          REQUIRE(mult2.underlying_value() == dval * 1.5);
        }
      }
      WHEN("multiply assigning") {
        quant *= 1.5;
        THEN(" it should be different") {
          REQUIRE(quant.underlying_value() == dval * 1.5);
        }
      }
    }
    WHEN("operating with integers") {
      WHEN("multiplying it with quantity on the left") {
        auto mult = quant * 2;
        THEN(" it should be different") {
          REQUIRE(mult.underlying_value() == dval * 2);
        }
      }

      WHEN("multiplying it with quantity on the right") {
        auto mult = 2 * quant;
        THEN(" it should be different") {
          REQUIRE(mult.underlying_value() == dval * 2);
        }
      }

      WHEN("dividing") {
        auto divided = quant / 2;
        THEN(" it should be different") {
          REQUIRE(divided.underlying_value() == dval / 2);
        }
      }

      WHEN("divide assign with ") {
        quant /= 2;
        THEN(" it should be different") {
          REQUIRE(quant.underlying_value() == dval / 2);
        }
      }
    }
    WHEN("copy constructing another quantity") {
      auto copy{quant};
      THEN(" the sum should be ok") {
        REQUIRE((copy + quant).underlying_value() == dval * 2);
      }
      THEN(" the difference should be ok") {
        REQUIRE((copy - quant).underlying_value() == 0.);
      }
    }
  }
}