#pragma once
#include <boost/hana.hpp>
#include <ratio>

namespace si {
    namespace Impl {
          template <class Arg>
  constexpr std::false_type is_ratio(Arg) {
    return std::false_type{};
  }

  template <intmax_t N, intmax_t D>
  constexpr std::true_type is_ratio(std::ratio<N, D>) {
    return std::true_type{};
  }
  
 template <intmax_t n = 1, intmax_t d = 1, class ratio=si::unity>
  struct BaseUnit{
    constexpr static auto exp = std::ratio<n, d>{};
    constexpr static auto prefix = ratio;
    static_assert(is_ratio(ratio));
  };
    
    
#define MAKE_BASE_UNIT(NAME, ABBREVIATION)                                     \
  template <intmax_t n = 1, intmax_t d = 1, class ratio=si::unity>                                    \
  struct NAME : BaseUnit<n, d,ratio> {                                               \
    using BaseUnit<n, d>::BaseUnit;                                            \
  };                                                                           \
  template <class Arg>                                                         \
  constexpr auto is_##NAME(Arg) {                                              \
    return std::false_type{};                                                  \
  }                                                                            \
  template <intmax_t n, intmax_t d, template<intmax_t, intmax_t> class ratio>                                            \
  constexpr auto is_##NAME(NAME<n, d, ratio>) {                                       \
    return std::true_type{};                                                   \
  }                                                                            \
  template <intmax_t n = 1, intmax_t d = 1, class Ratio=si::unity>                                    \
  using ABBREVIATION = NAME<n, d>;

  MAKE_BASE_UNIT(length, L)
  MAKE_BASE_UNIT(mass, M)
  MAKE_BASE_UNIT(time, Ti)
  MAKE_BASE_UNIT(luminosity, J)
  MAKE_BASE_UNIT(amount, N)
  MAKE_BASE_UNIT(current, I)
  MAKE_BASE_UNIT(temperature, Te)
  


  static_assert(is_length(length<1>{}));
  static_assert(is_length(length{}));
  static_assert(is_length(length<-1, 2>{}));
  static_assert(is_length(length<-1, 2, si::kilo>{}));

  static_assert(is_length(L<1>{}));
  static_assert(is_mass(mass<1>{}));
  static_assert(is_mass(M<1,2,si::kilo>{}));
  static_assert(!is_mass(length<1>{}));
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
    static_assert(is_base_dimension<temperature<1,2,si::mega>>());
  static_assert(!is_base_dimension<std::integral_constant<int, 1>>());
} // namespace si
