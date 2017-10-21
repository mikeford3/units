#pragma once
#include <ratio>
#include <boost/hana.hpp>

namespace si {
  template <intmax_t N=1, intmax_t D=1>
  struct BaseUnit {
    constexpr static auto exp = std::ratio<N, D>{};
  };
#define MAKE_BASE_UNIT(NAME)                                                   \
  template <intmax_t N=1, intmax_t D=1>                                                \
  struct NAME : BaseUnit<N, D> {                                           \
    using BaseUnit<N, D>::BaseUnit;                                        \
  };                                                                           \
  template <class Arg>                                                         \
  constexpr auto is_##NAME(Arg) {                                              \
    return std::false_type{};                                                  \
  }                                                                            \
  template <intmax_t N, intmax_t D>                                                             \
  constexpr auto is_##NAME(NAME<N, D>) {                                          \
    return std::true_type{};                                                   \
  }

  MAKE_BASE_UNIT(length)
  MAKE_BASE_UNIT(mass)
  MAKE_BASE_UNIT(time)
  MAKE_BASE_UNIT(luminosity)
  MAKE_BASE_UNIT(amount)
  MAKE_BASE_UNIT(current)
  MAKE_BASE_UNIT(temperature)

  static_assert(is_length(length<1>{}));
  static_assert(is_mass(mass<1>{}));
  static_assert(!is_mass(length<1>{}));

  template <class Arg>
  constexpr auto is_base_dimension(Arg arg) {
    if constexpr (is_length(arg) || is_mass(arg) || is_time(arg) ||
                  is_luminosity(arg) || is_amount(arg) || is_current(arg) ||
                  is_temperature(arg)) {
      return std::true_type{};
    } else {
      return std::false_type{};
    }
  }

  template <class Arg>
  constexpr auto is_base_dimension() {
    return is_base_dimension(Arg{});
  }
  static_assert(is_base_dimension(length{}));
  static_assert(is_base_dimension(mass{}));
  static_assert(is_base_dimension(time{}));
  static_assert(is_base_dimension(luminosity{}));
  static_assert(is_base_dimension(amount{}));
  static_assert(is_base_dimension(current{}));
  static_assert(is_base_dimension(temperature{}));
  static_assert(!is_base_dimension(std::integral_constant<int, 1>{}));

  static_assert(is_base_dimension<length<1>>());
  static_assert(is_base_dimension<mass<1>>());
  static_assert(is_base_dimension<time<1>>());
  static_assert(is_base_dimension<luminosity<1>>());
  static_assert(is_base_dimension<amount<1>>());
  static_assert(is_base_dimension<current<1>>());
  static_assert(is_base_dimension<temperature<1>>());
  static_assert(!is_base_dimension<std::integral_constant<int, 1>>());

} // namespace si
