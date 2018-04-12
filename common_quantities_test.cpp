#include "common_quantities.hpp"
#include <catch.hpp>
#include <random>
#include <vector>

SCENARIO("Common Quantities") {
  GIVEN("some common length quantities") {
    auto x0 = metre{1};
    auto x1 = metre{-11};
    WHEN("adding and subtracting") {
      THEN("1 + -11 is -10") { REQUIRE(x0 + x1 == metre{-10}); }
      THEN("1 - -11 is 12") { REQUIRE(x0 - x1 == metre{12}); }
    }
    WHEN("using abs") {
      REQUIRE(std::abs(x0) == x0);
      REQUIRE(std::abs(x1) == metre{11});
    }
    WHEN("using += and -=") {
      THEN("1 += -11 is -10") {
        x0 += x1;
        REQUIRE(x0 == metre{-10});
      }
      THEN("1 -= -11 is 12") {
        x0 -= x1;
        REQUIRE(x0 == metre{12});
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
    }
  }
  WHEN("Doing work with different prefixes") {
    GIVEN("Some length and area units") {
      using std::abs;
      using km2 = Quantity<si::derived_t<si::Length<2>, si::mega>>;
      using m2 = Quantity<si::derived_t<si::Length<2>>>;
      using m = metre;
      using mm = Quantity<si::derived_t<si::Length<1>, si::milli>>;
      using dimensionless = Quantity<si::derived_t<si::unity>>;
      WHEN("comparing lengths") {
        THEN("2 m < 2 km") { REQUIRE(m{2} < km{2}); }
        THEN("2 mm < 2 m") { REQUIRE(mm{2} < m{2}); }
        THEN("2 m > -2 km") { REQUIRE(m{2} > km{-2}); }
        THEN("2000 m >= 2 km") { REQUIRE(m{2000} >= km{2}); }
        THEN("2 km >= 2000 km") { REQUIRE(km{2} >= m{2000}); }
        THEN("2 km > 2 mm") { REQUIRE(km{2} > mm{2}); }
        THEN("2 km => 2 mm") { REQUIRE(km{2} >= mm{2}); }
        THEN("2 mm < 2 km") { REQUIRE(mm{2} < km{2}); }
        THEN("2 mm <= 2 km") { REQUIRE(mm{2} <= km{2}); }
        THEN("4,000,023 m == 4,000.023 km") {
          REQUIRE(abs(m{4'000'023} - m{4'000'023} <= m{1}));
          REQUIRE(abs(m{4'000'023} - km{4'000.023} <= m{1}));
          REQUIRE(abs(mm{1'000'050} - km{1.000'050} <= mm{0.1}));
        }
      }

      WHEN("adding and subtracting lengths") {
        WHEN(" using + ") {
          THEN("2 m + 2 m == 4 m") { REQUIRE(m{2} + m{2} == m{4}); }
          THEN("2 km + 2 m == 2,002 m") { REQUIRE(km{2} + m{2} == m{2'002}); }
          THEN("2 m + 2 km == 2,002 m") { REQUIRE(m{2} + km{2} == m{2'002}); }
          THEN("2 km + 2 m == 2.002 km") {
            REQUIRE(abs((km{2} + m{2}) - km{2.002}) <= m{0.001});
          }
          THEN("2 m + 2 km == 2.002 km") {
            REQUIRE(abs((m{2} + km{2}) - km{2.002}) <= m{0.001});
          }
        }
        WHEN(" using - ") {
          THEN("2 m - 1 m == 1 m") { REQUIRE(m{2} - m{1} == m{1}); }
          THEN("2 km - 2 m == 1,998 m") { REQUIRE(km{2} - m{2} == m{1'998}); }
          THEN("2 m - 2 km == -1'998 m") { REQUIRE(m{2} - km{2} == m{-1'998}); }
          THEN("2 km - 2 m == 1.998 km") {
            REQUIRE(abs((km{2} - m{2}) - km{1.998}) <= m{0.001});
          }
          THEN("2 m - 2 km == -1.998 km") {
            REQUIRE(abs((m{2} - km{2}) - km{-1.998}) <= m{0.001});
          }
        }

        WHEN(" using += ") {
          auto _2m = m{2};
          auto _2km = km{2};
          THEN("2 m += 2 m == 4 m") { REQUIRE((_2m += m{2}) == m{4}); }
          THEN("2 km += 2 m == 2,002 m") {
            REQUIRE(abs((_2km += m{2}) - m{2'002}) <= m{0.001});
          }
          THEN("2 m += 2 km == 2,002 m") {
            REQUIRE((_2m += km{2}) == m{2'002});
          }
          THEN("2 km += 2 m == 2.002 km") {
            REQUIRE(abs(_2km += m{2} - km{2.002}) <= m{0.001});
          }
          THEN("2 m += 2 km == 2.002 km") {
            REQUIRE(abs((_2m += km{2}) - km{2.002}) <= m{0.001});
          }
        }

        WHEN(" using -= ") {
          auto _2m = m{2};
          auto _2km = km{2};
          THEN("2 m -= 2 m == 4 m") { REQUIRE((_2m -= m{2}) == m{0}); }
          THEN("2 km -= 2 m == 1,998 m") {
            REQUIRE(abs((_2km -= m{2}) - m{1'998}) <= m{0.001});
          }
          THEN("2 m -= 2 km == -1,998 m") {
            REQUIRE((_2m -= km{2}) == m{-1'998});
          }
          THEN("2 km -= 2 m == 1.998 km") {
            REQUIRE(abs((_2km -= m{2}) - km{1.998}) <= m{0.001});
          }
          THEN("2 m -= 2 km == -1.998 km") {
            REQUIRE(abs((_2m -= km{2}) - km{-1.998}) <= m{0.001});
          }
        }
      }

      WHEN("dividing lengths by lengths") {
        WHEN("comparing to dimensionless quantities") {
        THEN("2 m / 2 m == 1 is true") { REQUIRE(m{2} / m{2} == dimensionless{1.}); }
        THEN("2 m / 2 m >= 1 is true") { REQUIRE(m{2} / m{2} >= dimensionless{1.}); }
        THEN("2 m / 2 m <= 1 is true") { REQUIRE(m{2} / m{2} <= dimensionless{1.}); }
        THEN("2 m / 2 m != 1 is false") { REQUIRE_FALSE(m{2} / m{2} != dimensionless{1.}); }
        THEN("2 m / 2 m >  1 is false") { REQUIRE_FALSE(m{2} / m{2} > dimensionless{1.}); }
        THEN("2 m / 2 m <  1 is false") { REQUIRE_FALSE(m{2} / m{2} < dimensionless{1.}); }

        THEN("3 m / 2 m >= 1 is true") { REQUIRE(m{3} / m{2} >= dimensionless{1.}); }
        THEN("3 m / 2 m >  1 is true") { REQUIRE(m{3} / m{2} > dimensionless{1.}); }
        THEN("3 m / 2 m != 1 is true") { REQUIRE(m{3} / m{2} != dimensionless{1.}); }
        THEN("3 m / 2 m <  1 is false") { REQUIRE_FALSE(m{3} / m{2} < dimensionless{1.}); }
        THEN("3 m / 2 m <= 1 is false") { REQUIRE_FALSE(m{3} / m{2} <= dimensionless{1.}); }
        THEN("3 m / 2 m == 1 is false") { REQUIRE_FALSE(m{3} / m{2} == dimensionless{1.}); }


        THEN("2 m / 3 m != 1 is true") { REQUIRE(m{2} / m{3} != dimensionless{1.}); }
        THEN("2 m / 3 m <  1 is true") { REQUIRE(m{2} / m{3} < dimensionless{1.}); }
        THEN("2 m / 3 m <= 1 is true") { REQUIRE(m{2} / m{3} <= dimensionless{1.}); }
        THEN("2 m / 3 m == 1 is false") { REQUIRE_FALSE(m{2} / m{3} == dimensionless{1.}); }
        THEN("2 m / 3 m >= 1 is false") { REQUIRE_FALSE(m{2} / m{3} >= dimensionless{1.}); }
        THEN("2 m / 3 m >  1 is false") { REQUIRE_FALSE(m{2} / m{3} > dimensionless{1.}); }
        }

                WHEN("comparing to integers") {
        THEN("2 m / 2 m == 1 is true") { REQUIRE(m{2} / m{2} == 1); }
        THEN("2 m / 2 m >= 1 is true") { REQUIRE(m{2} / m{2} >= 1); }
        THEN("2 m / 2 m <= 1 is true") { REQUIRE(m{2} / m{2} <= 1); }
        THEN("2 m / 2 m != 1 is false") { REQUIRE_FALSE(m{2} / m{2} != 1); }
        THEN("2 m / 2 m >  1 is false") { REQUIRE_FALSE(m{2} / m{2} > 1); }
        THEN("2 m / 2 m <  1 is false") { REQUIRE_FALSE(m{2} / m{2} < 1); }

        THEN("3 m / 2 m >= 1 is true") { REQUIRE(m{3} / m{2} >= 1); }
        THEN("3 m / 2 m >  1 is true") { REQUIRE(m{3} / m{2} > 1); }
        THEN("3 m / 2 m != 1 is true") { REQUIRE(m{3} / m{2} != 1); }
        THEN("3 m / 2 m <  1 is false") { REQUIRE_FALSE(m{3} / m{2} < 1); }
        THEN("3 m / 2 m <= 1 is false") { REQUIRE_FALSE(m{3} / m{2} <= 1); }
        THEN("3 m / 2 m == 1 is false") { REQUIRE_FALSE(m{3} / m{2} == 1); }


        THEN("2 m / 3 m != 1 is true") { REQUIRE(m{2} / m{3} != 1); }
        THEN("2 m / 3 m <  1 is true") { REQUIRE(m{2} / m{3} < 1); }
        THEN("2 m / 3 m <= 1 is true") { REQUIRE(m{2} / m{3} <= 1); }
        THEN("2 m / 3 m == 1 is false") { REQUIRE_FALSE(m{2} / m{3} == 1); }
        THEN("2 m / 3 m >= 1 is false") { REQUIRE_FALSE(m{2} / m{3} >= 1); }
        THEN("2 m / 3 m >  1 is false") { REQUIRE_FALSE(m{2} / m{3} > 1); }
        }

        WHEN("comparing to doubles") {
        THEN("2 m / 2 m == 1 is true") { REQUIRE(m{2} / m{2} == 1.); }
        THEN("2 m / 2 m >= 1 is true") { REQUIRE(m{2} / m{2} >= 1.); }
        THEN("2 m / 2 m <= 1 is true") { REQUIRE(m{2} / m{2} <= 1.); }
        THEN("2 m / 2 m != 1 is false") { REQUIRE_FALSE(m{2} / m{2} != 1.); }
        THEN("2 m / 2 m >  1 is false") { REQUIRE_FALSE(m{2} / m{2} > 1.); }
        THEN("2 m / 2 m <  1 is false") { REQUIRE_FALSE(m{2} / m{2} < 1.); }

        THEN("3 m / 2 m >= 1 is true") { REQUIRE(m{3} / m{2} >= 1.); }
        THEN("3 m / 2 m >  1 is true") { REQUIRE(m{3} / m{2} > 1.); }
        THEN("3 m / 2 m != 1 is true") { REQUIRE(m{3} / m{2} != 1.); }
        THEN("3 m / 2 m <  1 is false") { REQUIRE_FALSE(m{3} / m{2} < 1.); }
        THEN("3 m / 2 m <= 1 is false") { REQUIRE_FALSE(m{3} / m{2} <= 1.); }
        THEN("3 m / 2 m == 1 is false") { REQUIRE_FALSE(m{3} / m{2} == 1.); }


        THEN("2 m / 3 m != 1 is true") { REQUIRE(m{2} / m{3} != 1.); }
        THEN("2 m / 3 m <  1 is true") { REQUIRE(m{2} / m{3} < 1.); }
        THEN("2 m / 3 m <= 1 is true") { REQUIRE(m{2} / m{3} <= 1.); }
        THEN("2 m / 3 m == 1 is false") { REQUIRE_FALSE(m{2} / m{3} == 1.); }
        THEN("2 m / 3 m >= 1 is false") { REQUIRE_FALSE(m{2} / m{3} >= 1.); }
        THEN("2 m / 3 m >  1 is false") { REQUIRE_FALSE(m{2} / m{3} > 1.); }
        
        }        
      }
WHEN("multiplying and dividing by dimensionless quantities") {
      auto two = dimensionless{2};
      auto two_million = Quantity<si::derived_t<si::mega>>{2};
      
      THEN("2m *= 2m / 2m = 2m") {
        REQUIRE((m{2} *= m{2} / m{2}) == m{2});
      }
      THEN("2m *= 4m / 2m = 2m") {
        REQUIRE((m{2} *= m{4} / m{2}) == m{4});
      }

      THEN("2m *= two = 4m") {
        REQUIRE((m{2} *= two) == m{4});
      }
      THEN("2m *= two_million = 4,000,000m") {
        REQUIRE((m{2} *= two_million) == m{4'000'000});
        REQUIRE((m{2} *= two_million) == km{4'000});        
      }

      THEN("2m /= 2m / 2m = 2m") {
        REQUIRE((m{2} /= m{2} / m{2}) == m{2});
      }
      THEN("2m /= (4m / 2m) = 1m") {
        REQUIRE((m{2} /= (m{4} / m{2})) == m{1});
      }

            THEN("2m /= (2m / 4m) = 1m") {
        REQUIRE((m{2} /= (m{2} / m{4})) == m{4});
      }

      THEN("2m /= two = 4m") {
        REQUIRE((m{2} /= two) == m{1});
      }
      THEN("2m /= two_million = 4,000,000m") {
        REQUIRE((m{2} /= two_million) == m{1e-6});
        REQUIRE(abs((m{2} /= two_million) - km{1e-9}) <= m{1e-20});        
      }
}
      /*WHEN("using multiply") {

        THEN("2 m * 2 m = 4 m^2") { REQUIRE(m{2} * m{2} == m2{4}); }
        THEN("2 m * 2000 m = 4000 m^2") { REQUIRE(m{2} * m{2000} == m2{4000}); }
        THEN("2 m * -2 m = -4 m^2") { REQUIRE(m{2} * m{-2} == m2{-4}); }
        THEN("2 m * -2000 m = -4000 m^2") {
          REQUIRE(m{2} * m{-2000} == m2{-4000});
        }
        THEN("-2 m * -2 m = 4 m^2") { REQUIRE(m{-2} * m{-2} == m2{4}); }
        THEN("-2 m * -2000 m = 4000 m^2") {
          REQUIRE(m{-2} * m{-2000} == m2{4000});
        }

        THEN("2 m * 2 km = 4,000 m^2") { REQUIRE(m{2} * km{2} == m2{4000}); }
        THEN("2 km * 2 m = 4,000 m^2") { REQUIRE(km{2} * m{2} == m2{4000}); }

        THEN("2 km * 2 km = 4,000,000 m^2") {
          REQUIRE(km{2} * km{2} == m2{4000000});
        }
        THEN("2 km * 2 km = 4 km^2") { REQUIRE(km{2} * km{2} == km2{4}); }

        // THEN("20 km * 1 m = 20000 m^2 = 0.02 km^2") { REQUIRE(x2 * x1 ==) }
      }*/
    }
    /*
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
    }*/
  }
}

SCENARIO("Profiling the Quantity class", "[Profile]") {
  GIVEN("a random number generator to defeat the optimiser") {
    auto engine = std::default_random_engine{};
    auto dist = std::normal_distribution{};
    auto gen = std::bind(dist, engine);
    int num_runs = 10000000;
    auto vec = std::vector<double>(num_runs);
    std::generate(std::begin(vec), std::end(vec), gen);

    auto tot0 = Quantity<MPam05_t>{0};
    auto tot1 = 0.;
    for (auto i = 0; i < 10; ++i) {
      BENCHMARK("using Quantity") {
        for (auto i = 0; i < num_runs; ++i) {
          tot0 += Quantity<MPam05_t>{vec[i]};
        }
      }

      BENCHMARK("using double") {
        for (auto i = 0; i < num_runs; ++i) {
          tot1 += vec[i];
        }
      }
    }
    std::cout << "\n" << tot0 << "\t" << tot1 << "\n";
  }
}
