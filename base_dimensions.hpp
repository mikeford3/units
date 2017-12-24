#pragma once

#include "prefixes.hpp"
#include <boost/hana.hpp>
#include <ratio>
#include <type_traits>

namespace si {
  // namespace Impl {
  template <class Arg>
  constexpr std::false_type is_ratio(Arg) {
    return std::false_type{};
  }

  template <intmax_t N, intmax_t D>
  constexpr std::true_type is_ratio(std::ratio<N, D>) {
    return std::true_type{};
  }

  template <intmax_t n = 1, intmax_t d = 1, class ratio = si::unity>
  struct BaseUnit {
    constexpr static auto exp = std::ratio<n, d>{};
    constexpr static auto prefix = ratio{};
    static_assert(is_ratio(ratio{}));
  };

#define MAKE_BASE_UNIT(NAME, ABBREVIATION, LOWER)                              \
  template <intmax_t n = 1, intmax_t d = 1, class ratio = si::unity>           \
  struct NAME : BaseUnit<n, d, ratio> {                                        \
    using BaseUnit<n, d, ratio>::BaseUnit;                                     \
  };                                                                           \
  template <class Arg>                                                         \
  constexpr auto is_##LOWER(Arg) {                                             \
    return std::false_type{};                                                  \
  }                                                                            \
  template <intmax_t n, intmax_t d, class ratio>                               \
  constexpr auto is_##LOWER(NAME<n, d, ratio>) {                               \
    return std::true_type{};                                                   \
  }                                                                            \
  template <intmax_t n = 1, intmax_t d = 1, class Ratio = si::unity>           \
  using ABBREVIATION = NAME<n, d>;

  MAKE_BASE_UNIT(Length, L, length)
  MAKE_BASE_UNIT(Mass, M, mass)
  MAKE_BASE_UNIT(Time, Ti, time)
  MAKE_BASE_UNIT(Luminosity, J, luminosity)
  MAKE_BASE_UNIT(Amount, N, amount)
  MAKE_BASE_UNIT(Current, I, current)
  MAKE_BASE_UNIT(Temperature, Te, temperature)

  static_assert(is_length(Length<1>{}));
  static_assert(is_length(Length{}));
  static_assert(is_length(Length<-1, 2>{}));
  static_assert(is_length(Length<-1, 2, si::kilo>{}));

  static_assert(is_length(L<1>{}));
  static_assert(is_mass(Mass<1>{}));
  static_assert(is_mass(M<1, 2, si::kilo>{}));
  static_assert(!is_mass(Length<1>{}));
  static_assert(!is_mass(L<1>{}));

  template <class Arg>
  constexpr auto is_base_dimension(Arg arg = Arg{}) {
    if constexpr (is_length(arg) || is_mass(arg) || is_time(arg) ||
                  is_luminosity(arg) || is_amount(arg) || is_current(arg) ||
                  is_temperature(arg)) {
      return std::true_type{};
    } else {
      return std::false_type{};
    }
  }

  static_assert(is_base_dimension(Length{}));
  static_assert(is_base_dimension(Mass{}));
  static_assert(is_base_dimension(Time{}));
  static_assert(is_base_dimension(Luminosity{}));
  static_assert(is_base_dimension(Amount{}));
  static_assert(is_base_dimension(Current{}));
  static_assert(is_base_dimension(Temperature{}));
  static_assert(!is_base_dimension(std::integral_constant<int, 1>{}));

  static_assert(is_base_dimension<Length<1>>());
  static_assert(is_base_dimension<Mass<1>>());
  static_assert(is_base_dimension<Time<1>>());
  static_assert(is_base_dimension<Luminosity<1>>());
  static_assert(is_base_dimension<Amount<1>>());
  static_assert(is_base_dimension<Current<1>>());
  static_assert(is_base_dimension<Temperature<1>>());
  static_assert(is_base_dimension<Temperature<1, 2, si::mega>>());
  static_assert(!is_base_dimension<std::integral_constant<int, 1>>());
} // namespace si
