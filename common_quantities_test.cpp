#include "common_quantities.hpp"
#include <catch.hpp>

SCENARIO("Common Quantities") {
  GIVEN("some common length quantities") {
    auto x0 = metre{1};
    auto x1 = metre{-11};
    auto x2 = km{20};
    WHEN("adding and subtracting") {
      THEN("1 + -11 is -10") { REQUIRE(x0 + x1 == metre{-10}); }
      THEN("1 - -11 is 12") { REQUIRE(x0 - x1 == metre{12}); }
    }
    WHEN("using abs") {
      REQUIRE(std::abs(x0) == x0);
      REQUIRE(std::abs(x1) == metre{11});
    }
    WHEN("using comparisons") {
      REQUIRE(x0 > metre{-1.5});
      REQUIRE(x0 > metre{0.5});
      REQUIRE_FALSE(x0 > metre{1});
      REQUIRE_FALSE(x0 > metre{1.5});

      REQUIRE(x0 >= metre{-1.5});
      REQUIRE(x0 >= metre{0.5});
      REQUIRE(x0 >= metre{1});
      REQUIRE_FALSE(x0 >= metre{1.5});

      REQUIRE_FALSE(x0 < metre{-1.5});
      REQUIRE_FALSE(x0 < metre{0.5});
      REQUIRE_FALSE(x0 < metre{1});
      REQUIRE(x0 < metre{1.5});

      REQUIRE_FALSE(x0 <= metre{-1.5});
      REQUIRE_FALSE(x0 <= metre{0.5});
      REQUIRE(x0 <= metre{1});
      REQUIRE(x0 <= metre{1.5});
    }
    WHEN("using MPam^0.5") {
      auto a = Quantity<seconds_t>{100.3};
      auto b = Quantity<kg_t>{1};
      auto c1 = Quantity<velocity_t1>{20};
      auto c2 = Quantity<velocity_t2>{20};

      auto d = Quantity<acceleration_t>{20};
      auto e = Quantity<N_t>{30};
      auto f = Quantity<Pa_t>{15};
      auto g = Quantity<MPam05_t>{20};
      std::cout << "a = " << a << "\n";
      std::cout << "b = " << b << "\n";
      std::cout << "c1 = " << c1 << "\n";
      std::cout << "c2 = " << c2 << "\n";
      std::cout << "d = " << d << "\n";
      std::cout << "e = " << e << "\n";
      std::cout << "f = " << f << "\n";
      std::cout << "g = " << g << "\n";
    }
  }
}
