#pragma once
#include "base_dimensions.hpp"
#include <boost/hana.hpp>
#include <cassert>
#include <iostream>
#include <limits>
#include <locale>

namespace si {
  template <class Arg>
  constexpr std::false_type is_ratio(Arg) {
    return std::false_type{};
  }

  template <intmax_t N, intmax_t D>
  constexpr std::true_type is_ratio(std::ratio<N, D>) {
    return std::true_type{};
  }

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

  template <intmax_t digit>
  constexpr std::string to_superscript() {
    static_assert(digit >= 0);
    if constexpr (digit == 0)
      return u8"\u2070";
    else if constexpr (digit == 1)
      return u8"\u00B9";
    else if constexpr (digit == 2)
      return u8"\u00B2";
    else if constexpr (digit == 3)
      return u8"\u00B3";
    else if constexpr (digit == 4)
      return u8"\u2074";
    else if constexpr (digit == 5)
      return u8"\u2075";
    else if constexpr (digit == 6)
      return u8"\u2076";
    else if constexpr (digit == 7)
      return u8"\u2077";
    else if constexpr (digit == 8)
      return u8"\u2078";
    else if constexpr (digit == 9)
      return u8"\u2079";
     else
      return to_superscript<digit / 10>() + to_superscript<digit % 10>();
  }

  template <intmax_t N, intmax_t D>
  constexpr std::string print_unit(std::ratio<N, D>, std::string base_unit) {
    using namespace std::string_literals;
    if constexpr (D == 0) {
      if constexpr (N == 0) {
        return "";
      } else {
        return base_unit + "^" +
               std::to_string(std::numeric_limits<float>::infinity());
      }
    }
    // if whole number
    // else if nice fraction, square root etc
    // else nasty number 1.23532,,
    if constexpr (N % D == 0) {
      if constexpr (N / D > 0)
        return base_unit + to_superscript<N / D>();
      else
        return base_unit + u8"\u207B" + to_superscript<-N / D>();
    } else if constexpr (D % N == 0 && N > 0 && D > 0) {
      // std::cout << u8"square root = '" << u8"\u221A" << "'\n";
      if constexpr (D / N == 2)
        return u8"\u221A" + base_unit;
      else if constexpr (D / N == 3)
        return u8"\u221B" + base_unit;
      else if constexpr (D / N == 4)
        return u8"\u221C" + base_unit;
    } else {
      return base_unit + "^" + std::to_string(static_cast<double>(N) / D);
    }
  } // namespace si

  template <class L, class M, class T, class C, class Te, class A, class Lu>
  std::ostream& operator<<(std::ostream& os,
                           Dimensions<L, M, T, C, Te, A, Lu>) {
    using type = Dimensions<L, M, T, C, Te, A, Lu>;
    if constexpr (type::length::num != 0)
      os << print_unit(L{}, "m");
    if constexpr (type::mass::num != 0)
      os << print_unit(M{}, "kg");
    if constexpr (type::time::num != 0)
      os << print_unit(T{}, "s");
    if constexpr (type::current::num != 0)
      os << print_unit(C{}, "A");
    if constexpr (type::temperature::num != 0)
      os << print_unit(Te{}, "K");
    if constexpr (type::amount::num != 0)
      os << print_unit(A{}, "mol");
    if constexpr (type::luminosity::num != 0)
      os << print_unit(Lu{}, "cd");

    return os;
  }

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
      /*
            template<class rtrat>
            constexpr auto to_std_ratio<runtime_ratio rat>() {
              constexpr auto N = rat.n;
              return std::ratio<rat.n, rat.d>{};
            }*/
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
      } else if (is_temperature(arg)) {
        count.temperature += Arg::exp;
      } else if (is_amount(arg)) {
        count.amount += Arg::exp;
      } else if (is_luminosity(arg)) {
        count.luminosity += Arg::exp;
      } else if (is_current(arg)) {
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
        assert(false);
      }
      return count;
    }

    template <class... Args>
    constexpr auto parse_units(Args... args) {
      auto count = DimensionCounter{};
      constexpr auto tup_args = boost::hana::make_tuple(args...);
      boost::hana::for_each(
          tup_args, [&count](auto arg) { count += Impl::parse_arg(arg); });
      return count;
    }

    template <class... Args>
    constexpr auto parse_units() {
      auto count = DimensionCounter{};
      constexpr auto args = boost::hana::tuple<Args...>{};
      boost::hana::for_each(
          args, [&count](auto arg) { count += Impl::parse_arg(arg); });

      return count;
    }
  } // namespace Impl

  template <class... Args>
  constexpr auto parse_units(Args... args) {
    constexpr auto count = Impl::parse_units(args...);
    return Dimensions<std::ratio<count.length.n, count.length.d>,
                      std::ratio<count.mass.n, count.mass.d>,
                      std::ratio<count.time.n, count.time.d>,
                      std::ratio<count.current.n, count.current.d>,
                      std::ratio<count.temperature.n, count.temperature.d>,
                      std::ratio<count.amount.n, count.amount.d>,
                      std::ratio<count.luminosity.n, count.luminosity.d>>{};
  }

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

  template <class... Args>
  struct derived : decltype(parse_units<Args...>()) {

    constexpr static auto Units = boost::hana::tuple<Args...>{};
    static_assert(boost::hana::all_of(Units, [](auto arg) {
      return is_base_dimension(arg) || is_derived(arg);
    }));
    // static_assert(is_base_dimension<Args...>);
    // : public decltype(parse_units<Args...>()) {
  };

  template <class... Args>
  derived(Args... args)->derived<Args...>;

  static_assert(!is_derived(length<1>{}));
  static_assert(is_derived(derived<length<1>>{}));
  static_assert(is_derived(derived<length<1>, mass<1>>{}));
} // namespace si