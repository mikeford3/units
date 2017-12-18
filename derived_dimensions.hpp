#pragma once
#include "base_dimensions.hpp"
#include <boost/hana.hpp>
#include <cassert>
#include <exception>

namespace si {
  template <class Arg>
  constexpr std::false_type is_ratio(Arg) {
    return std::false_type{};
  }

  template <intmax_t N, intmax_t D>
  constexpr std::true_type is_ratio(std::ratio<N, D>) {
    return std::true_type{};
  }
  /** Compile time class which holds a std::ratio for each */
  template <class Length, class Mass, class Time, class Current,
            class Temperature, class Amount, class Luminosity>
  struct Dimensions {
    using length = Length;
    using mass = Mass;
    using time = Time;
    using current = Current;
    using temperature = Temperature;
    using amount = Amount;
    using luminosity = Luminosity;
    static_assert(is_ratio(length{}));
    static_assert(is_ratio(mass{}));
    static_assert(is_ratio(time{}));
    static_assert(is_ratio(current{}));
    static_assert(is_ratio(temperature{}));
    static_assert(is_ratio(amount{}));
    static_assert(is_ratio(luminosity{}));
  };

  // Fwd declartion
  template <class... Args>
  struct derived;

  template <class Arg>
  constexpr std::false_type is_derived(Arg) {
    return std::false_type{};
  }

  template <class... Args>
  constexpr std::true_type is_derived(derived<Args...>) {
    return std::true_type{};
  }

  namespace Impl {
    struct DimensionCounter {
      struct runtime_ratio {
        intmax_t n;
        intmax_t d;
        constexpr runtime_ratio() : n{0}, d{1} {}
        constexpr runtime_ratio(intmax_t a, intmax_t b) : n{a}, d{b} {}
        constexpr runtime_ratio& operator+=(const runtime_ratio& other) {
          n = n * other.d + other.n * d;
          d *= other.d;
          return *this;
        }
        template <intmax_t N, intmax_t D>
        constexpr runtime_ratio& operator+=(std::ratio<N, D>) {
          n = n * D + N * d;
          d *= D;
          return *this;
        }
      };
      runtime_ratio length{0, 1};
      runtime_ratio mass{0, 1};
      runtime_ratio time{0, 1};
      runtime_ratio current{0, 1};
      runtime_ratio temperature{0, 1};
      runtime_ratio amount{0, 1};
      runtime_ratio luminosity{0, 1};
      constexpr DimensionCounter& operator+=(const DimensionCounter& other) {
        length += other.length;
        mass += other.mass;
        time += other.time;
        current += other.current;
        temperature += other.temperature;
        amount += other.amount;
        luminosity += other.luminosity;
        return *this;
      }
    };

    template <class Arg>
    constexpr DimensionCounter parse_base_unit(Arg arg) {
      auto count = DimensionCounter{};
      if constexpr (is_length(arg)) {
        count.length += Arg::exp;
      } else if constexpr (is_time(arg)) {
        count.time += Arg::exp;
      } else if constexpr (is_mass(arg)) {
        count.mass += Arg::exp;
      } else if constexpr (is_temperature(arg)) {
        count.temperature += Arg::exp;
      } else if constexpr (is_amount(arg)) {
        count.amount += Arg::exp;
      } else if constexpr (is_luminosity(arg)) {
        count.luminosity += Arg::exp;
      } else if constexpr (is_current(arg)) {
        count.current += Arg::exp;
      } else {
        assert(false);
      }
      return count;
    } // namespace Impl

    template <class Arg>
    constexpr DimensionCounter parse_arg(Arg arg) {
      auto count = DimensionCounter{};
      if constexpr (si::is_derived(arg)) {
        boost::hana::for_each(Arg::Units, [&count](auto subarg) {
          count += Impl::parse_arg(subarg);
        });
      } else if constexpr (si::is_base_dimension(arg)) {
        count += parse_base_unit(arg);
      } else {
        // throw std::logic_error("Oops");
        // std::cout << "Oops\n";
        // assert(false);
      }
      return count;
    }

    /** At compile time create a dimension counter and add all of the Args to
     * it. */
    template <class... Args>
    constexpr auto parse_units() {
      auto count = DimensionCounter{};
      constexpr auto args = boost::hana::tuple<Args...>{};
      boost::hana::for_each(
          args, [&count](auto arg) { count += Impl::parse_arg(arg); });

      return count;
    }
  } // namespace Impl

  /** Collect a number of derived or base dimension classes into a single one.
   */
  template <class... Args>
  constexpr auto parse_units() {
    constexpr auto count = Impl::parse_units<Args...>();
    return Dimensions<std::ratio<count.length.n, count.length.d>,
                      std::ratio<count.mass.n, count.mass.d>,
                      std::ratio<count.time.n, count.time.d>,
                      std::ratio<count.current.n, count.current.d>,
                      std::ratio<count.temperature.n, count.temperature.d>,
                      std::ratio<count.amount.n, count.amount.d>,
                      std::ratio<count.luminosity.n, count.luminosity.d>>{};
  }
  /** Collect a number of dereived or base dimension classes into a single one
   * using the parse_units constexpr function */
  template <class... Args>
  struct derived : decltype(parse_units<Args...>()) {
    constexpr static auto Units = boost::hana::tuple<Args...>{};
    static_assert(boost::hana::all_of(Units, [](auto arg) {
      return is_base_dimension(arg) || is_derived(arg);
    }));
    // static_assert(is_base_dimension<Args...>);
    // : public decltype(parse_units<Args...>()) {
  };
  template <template <class... Args0>, template <class... Args1>>
  constexpr auto operator*(derived<Args0...> a, derived<Args1...> b) {
    return false;
  }

      // Deduction guide
      template <class... Args>
      derived(Args... args) -> derived<Args...>;

  static_assert(!is_derived(length<1>{}));
  static_assert(is_derived(derived<length<1>>{}));
  static_assert(is_derived(derived<length<1>, mass<1>>{}));

} // namespace si