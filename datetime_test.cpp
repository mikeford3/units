#include "datetime.hpp"
#include <catch.hpp>

TEST_CASE("Checking that the chrono conversions work") {
  using std::to_string;
  auto lots_of_seconds = Seconds{120};
  REQUIRE(Minutes(lots_of_seconds).count() == 2);
  auto day = Days{1};
  auto hour = Hours{1};
  auto min = Minutes{1};
  auto sec = Seconds{1};
  std::cout << "\n"
            << to_string(day) << "\n"
            << to_string(hour) << "\n"
            << to_string(min) << "\n"
            << to_string(sec) << "\n";

  std::cout << to_string(Minutes{181}) << "\n";
}