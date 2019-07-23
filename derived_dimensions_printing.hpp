#pragma once

#include "derived_dimensions.hpp"
#include <cmath>
#include <limits>
#include <locale>
#include <ostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <string_constants.hpp>

namespace units {

  // template <size_t N>
  // std::ostream& operator<<(std::ostream& os, StringConstant<N> sc) {
  //   for (int i = 0; i < sc.Length(); ++i) {
  //     os << sc[i];
  //   }
  //   return os;
  // }

  /** Takes an int as a template arg and returns the superscript chars, use to
   *  print powers, e.g m^2.*/
  template <intmax_t digit>
  constexpr auto to_integer_superscript() {
    if constexpr (digit < 0) {
      assert(false);
      return "";
    }
    if constexpr (digit == 0)
      return StringFactory("\u2070");
    else if constexpr (digit == 1)
      return StringFactory("\u00B9");
    else if constexpr (digit == 2)
      return StringFactory("\u00B2");
    else if constexpr (digit == 3)
      return StringFactory("\u00B3");
    else if constexpr (digit == 4)
      return StringFactory("\u2074");
    else if constexpr (digit == 5)
      return StringFactory("\u2075");
    else if constexpr (digit == 6)
      return StringFactory("\u2076");
    else if constexpr (digit == 7)
      return StringFactory("\u2077");
    else if constexpr (digit == 8)
      return StringFactory("\u2078");
    else if constexpr (digit == 9)
      return StringFactory("\u2079");
    else
      return to_integer_superscript<digit / 10>() +
             to_integer_superscript<digit % 10>();
  }

  /** Similar to above but takes runtime args.
   *  TODO - if constepxr containers are added to the standard than make this
   *  constexpr and delete the template function above (P0784?).
   */
  inline std::string to_integer_superscript(int digit) {
    if (digit < 0) {
      assert(false);
      return "";
    }
    using namespace std::string_literals;
    if (digit == 0)
      return "\u2070"s;
    else if (digit == 1)
      return "\u00B9";
    else if (digit == 2)
      return "\u00B2";
    else if (digit == 3)
      return "\u00B3";
    else if (digit == 4)
      return "\u2074";
    else if (digit == 5)
      return "\u2075";
    else if (digit == 6)
      return "\u2076";
    else if (digit == 7)
      return "\u2077";
    else if (digit == 8)
      return "\u2078";
    else if (digit == 9)
      return "\u2079";
    else
      return to_integer_superscript(digit / 10) +
             to_integer_superscript(digit % 10);
  }

  /** Return symbol for sqrt, cbrt, 4th root */
  template <intmax_t N, intmax_t D>
  constexpr auto to_root() {
    static_assert(D / N <= 4 && D / N >= 2);
    if constexpr (D / N == 2)
      return StringFactory("\u221A");
    else if constexpr (D / N == 3)
      return StringFactory("\u221B");
    else if constexpr (D / N == 4)
      return StringFactory("\u221C");
  }

  /** Return string for power that is a fraction */
  template <intmax_t N, intmax_t D>
  constexpr auto to_fractional(std::integral_constant<intmax_t, N>,
                               std::integral_constant<intmax_t, D>) {
    auto number = to_integer_superscript<0>() + "\u22C5" +
                  to_integer_superscript<100 / (D / N)>();
    return number;
  }

  /** Add the power to the baseUnit as a suffix, or put a symbol before the
   * baseUnit such as crbt. Returns a StringConstant of the correct length. For
   * example: baseUnit = "m", N = 3 and D = 1 returns "m^3"  (but with 3 as a
   * superscript) or baseUnit = "kg", N = 1 and D = 2 returns "sqrtkg", but with
   * the symbol for sqrt
   */
  template <intmax_t N, intmax_t D, size_t N1>
  constexpr auto print_unit(std::ratio<N, D>, StringConstant<N1> baseUnit) {
    using namespace std::string_literals;
    if constexpr (D == 0) {
      static_assert(N == 0);
      return StringFactory("");
    }

    constexpr auto positive [[maybe_unused]] = [&] { return (N * D > 0); }();
    constexpr auto negative [[maybe_unused]] = [&] { return (N * D < 0); }();
    constexpr auto root_symbol [[maybe_unused]] = [&] {
      return D / N == 2 || D / N == 3 || D / N == 4;
    }();
    constexpr auto int_power = [&] { return N % D == 0; }();
    constexpr auto nice_fraction
        [[maybe_unused]] = [&] { return D % N == 0; }();
    constexpr auto sign [[maybe_unused]] = []() {
      if constexpr (positive)
        return StringFactory("");
      else
        return StringFactory("\u207B");
    }();
    constexpr auto absN [[maybe_unused]] = positive ? N : -N;
    constexpr auto absNc [[maybe_unused]] =
        std::integral_constant<intmax_t, absN>{};
    constexpr auto Nc [[maybe_unused]] = std::integral_constant<intmax_t, N>{};
    constexpr auto Dc [[maybe_unused]] = std::integral_constant<intmax_t, D>{};

    if constexpr (int_power) {
      if constexpr (N / D == 1) {
        return baseUnit;
      } else {
        return baseUnit + sign + to_integer_superscript<absN / D>();
      }
    } else if constexpr (nice_fraction) {
      if constexpr (root_symbol) {
        return to_root<absN, D>() + baseUnit;
      } else {
        return baseUnit + sign + to_fractional(absNc, Dc);
      }
    } else {
      {
        return baseUnit + sign + to_integer_superscript<0>() + "\u22C5" +
               to_integer_superscript<absN * 100 / D>();
      }
    }
  }

} // namespace units

/** Print the Dimension class, e.g. something like "km" or "ms", including
 *  superscripts for to the power of.
 */
template <class L, class M, class T, class C, class Te, class A, class Lu,
          class Pr>
std::ostream& operator<<(std::ostream& os,
                         const units::Dimensions<L, M, T, C, Te, A, Lu, Pr>&) {
  using namespace units;
  using type = units::Dimensions<L, M, T, C, Te, A, Lu, Pr>;
  using prefix = typename type::prefix;

  // Print k, M, G etc for kilo, mega, giga ...
  bool prefixed = true;
  if constexpr (std::ratio_not_equal_v<prefix, units::unity>) {
    if constexpr (std::ratio_equal_v<prefix, units::kilo>)
      os << "k";
    else if constexpr (std::ratio_equal_v<prefix, units::mega>)
      os << "M";
    else if constexpr (std::ratio_equal_v<prefix, units::giga>)
      os << "G";
    else if constexpr (std::ratio_equal_v<prefix, units::milli>)
      os << "m";
    else if constexpr (std::ratio_equal_v<prefix, units::centi>)
      os << "c";
    else
      prefixed = false;
  }
  // print the dimension
  if constexpr (type::length::num != 0)
    os << print_unit(L{}, StringFactory("m"));
  if constexpr (type::mass::num != 0)
    os << print_unit(M{}, StringFactory("kg"));
  if constexpr (type::time::num != 0)
    os << print_unit(T{}, StringFactory("s"));
  if constexpr (type::current::num != 0)
    os << print_unit(C{}, StringFactory("A"));
  if constexpr (type::temperature::num != 0)
    os << print_unit(Te{}, StringFactory("K"));
  if constexpr (type::amount::num != 0)
    os << print_unit(A{}, StringFactory("mol"));
  if constexpr (type::luminosity::num != 0)
    os << print_unit(Lu{}, StringFactory("cd"));

  // if no prefix (k, M) etc then maybe we should add "x 10 ^ ?"
  if (!prefixed) {
    if constexpr (type::prefix::num == type::prefix::den) {
      // nothing, x
    } else if constexpr (type::prefix::num / type::prefix::den != 0) {
      auto pw = std::log10(type::prefix::num / type::prefix::den);
      auto intpw = static_cast<int>(pw);
      if (std::fabs(pw / intpw - 1.) < 0.001) {
        os << " x 10" << units::to_integer_superscript(intpw);
      } else {
        os << " x " << type::prefix::num / type::prefix::den;
      }
    } else {
      os << " x " << static_cast<double>(type::prefix::num) / type::prefix::den;
    }
  }
  return os;
}