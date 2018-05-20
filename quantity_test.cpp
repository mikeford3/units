#include "common_quantities.hpp"
#include "quantity.hpp"
#include <catch.hpp>
#include <iostream>

SCENARIO("Testing tags") {
  GIVEN("Some simple tags") {
    class Tagged;
    class Tagged2;
    using default_type = Quantity<units::derived_t<units::L<1>>>;
    using tagged_type = Quantity<units::derived_t<units::L<1>>, double, Tagged>;
    WHEN("using multiplcation") {
      WHEN("Multiplying the tagged_type by the same tagged_type") {
        auto res = tagged_type{1} * tagged_type{2};
        THEN("the tag stays the same") {
          static_assert(std::is_same_v<decltype(res)::Tag, Tagged>);
        }
      }
      WHEN("Multiplying the untagged by another untagged") {
        auto res = default_type{1} * default_type{2};
        THEN("the tag stays the same") {
          static_assert(std::is_same_v<decltype(res)::Tag, std::false_type>);
        }
      }
      WHEN("Multiplying the tagged by a untagged") {
        auto res = default_type{1} * tagged_type{2};
        THEN("the tag stays the same") {
          static_assert(std::is_same_v<decltype(res)::Tag, Tagged>);
        }
      }
      WHEN("Multiplying the untagged by a tagged") {
        auto res = tagged_type{1} * default_type{2};
        THEN("the tag stays the same") {
          static_assert(std::is_same_v<decltype(res)::Tag, Tagged>);
        }
      }
      WHEN("Multiplying the tagged_type by a different tagged_type, should "
           "cause compilation error") {
        // using tagged_type2 =
        //    Quantity<units::derived_t<units::L<1>>, double, Tagged2>;

        // auto res = tagged_type{1} * tagged_type2{2};
        THEN("the tag stays the same") {
          // static_assert(std::is_same_v<decltype(res)::Tag, Tagged>);
        }
      }
    }
  }
}

SCENARIO("Test pow") {
  WHEN("raising metres") {
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
}

SCENARIO("Testing is quantity") {
  GIVEN("A quantity and a non-quantity object") {
    constexpr auto quant = Quantity<units::derived_t<units::L<1>>>{2};
    constexpr auto not_quant = 2.;
    THEN("the quant is a quantity") { static_assert(is_quantity(quant)); }
    THEN("the non-quant is not a quantity") {
      static_assert(!is_quantity(not_quant));
    }
  }
}

SCENARIO("Create some quantities of physical units") {
  using metre_l = units::derived_t<units::L<1>>;
  GIVEN("a quantity with only a unit type, default otherwise") {
    const double dval = 10.;
    auto quant = Quantity<metre_l>{dval};
    REQUIRE(quant.underlying_value() == dval);
    WHEN("multiply a quanity by itself") {
      auto quant_sq = quant * quant;
      THEN("the units must have changed") {
        // std::cout << quant;
        std::cout << quant << " * " << quant << " = " << quant_sq << "\n";
        static_assert(
            std::is_same_v<Quantity<units::derived_t<units::Length<2>>>,
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

      WHEN("dividing Quant by int") {
        auto divided = quant / 2;
        THEN(" it should be different") {
          REQUIRE(divided.underlying_value() == dval / 2);
        }
      }

      WHEN("dividing int by Quant") {
        auto divided = 1 / quant;
        THEN(" it should be different") {
          REQUIRE(divided.underlying_value() == 1 / dval);
          REQUIRE(divided ==
                  Quantity<units::derived_t<units::Length<-1, 1>>>{1 / dval});
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