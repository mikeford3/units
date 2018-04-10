#pragma once

#include "derived_dimensions.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <locale>
#include <stdio.h>
#include <string.h>

namespace si {
  template <intmax_t digit>
  constexpr std::string to_integer_superscript() {
    if constexpr (digit < 0) {
      assert(false);
      return "";
    }
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
      return to_integer_superscript<digit / 10>() +
             to_integer_superscript<digit % 10>();
  }

  template <intmax_t N, intmax_t D>
  constexpr std::string to_root() {
    static_assert(D / N <= 4 && D / N >= 2);
    if constexpr (D / N == 2)
      return u8"\u221A";
    else if constexpr (D / N == 3)
      return u8"\u221B";
    else if constexpr (D / N == 4)
      return u8"\u221C";
  }

  template <intmax_t N, intmax_t D>
  constexpr std::string to_fractional(std::integral_constant<intmax_t, N>,
                                      std::integral_constant<intmax_t, D>) {

    auto number = to_integer_superscript<0>() + u8"\u22C5" +
                  to_integer_superscript<100 / (D / N)>();
    return number;
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
    constexpr auto positive[[maybe_unused]] = [&] { return N > 0 && D > 0; };
    constexpr auto negative[[maybe_unused]] = [&] { return !positive(); };
    constexpr auto root_symbol[[maybe_unused]] = [&] {
      return D / N == 2 || D / N == 3 || D / N == 4;
    };
    constexpr auto int_power = [&] { return N % D == 0; };
    constexpr auto nice_fraction[[maybe_unused]] = [&] { return D % N == 0; };
    constexpr auto sign[[maybe_unused]] = positive() ? "" : u8"\u207B";
    constexpr auto absN[[maybe_unused]] = positive() ? N : -N;
    constexpr auto absNc[[maybe_unused]] =
        std::integral_constant<intmax_t, absN>{};
    constexpr auto Nc[[maybe_unused]] = std::integral_constant<intmax_t, N>{};
    constexpr auto Dc[[maybe_unused]] = std::integral_constant<intmax_t, D>{};

    if constexpr (int_power()) {
      if constexpr (N / D == 1)
        return base_unit;
      return base_unit + sign + to_integer_superscript<absN / D>();
    } else if constexpr (nice_fraction()) {
      if constexpr (root_symbol())
        return to_root<absN, D>() + base_unit;
      return base_unit + sign + to_fractional(absNc, Dc);
    } else {
      return base_unit + sign + to_integer_superscript<0>() + u8"\u22C5" +
             to_integer_superscript<absN * 100 / D>();
    }
  }

} // namespace si

template <class L, class M, class T, class C, class Te, class A, class Lu,
          class Pr>
std::ostream& operator<<(std::ostream& os,
                         const si::Dimensions<L, M, T, C, Te, A, Lu, Pr>&) {
  using namespace si;
  using type = si::Dimensions<L, M, T, C, Te, A, Lu, Pr>;
  using prefix = typename type::prefix;
  bool prefixed = true;
  if constexpr (std::ratio_not_equal_v<prefix, si::unity>) {
    if constexpr (std::ratio_equal_v<prefix, si::kilo>)
      os << "k";
    else if constexpr (std::ratio_equal_v<prefix, si::mega>)
      os << "M";
    else if constexpr (std::ratio_equal_v<prefix, si::giga>)
      os << "G";
    else if constexpr (std::ratio_equal_v<prefix, si::milli>)
      os << "m";
    else if constexpr (std::ratio_equal_v<prefix, si::centi>)
      os << "c";
    else
      prefixed = false;
  }
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
  if (!prefixed) {
    if constexpr (type::prefix::num == type::prefix::den) {
      //nothing, x
    } else if constexpr (type::prefix::num / type::prefix::den != 0) {
      constexpr auto pw = std::log10(type::prefix::num / type::prefix::den);
      constexpr auto intpw = static_cast<int>(pw);
      if constexpr (std::fabs(pw / intpw - 1.) < 0.001) {
        os << " x 10" << to_integer_superscript<intpw>();
      } else {
        os << " x " << type::prefix::num / type::prefix::den;
      }
    } else {
      os << " x " << static_cast<double>(type::prefix::num) / type::prefix::den;
    }
  }
  return os;
}