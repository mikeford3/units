#pragma once

#include "quantity.hpp"
#include <experimental/type_traits>
#include <limits>

// ************************************************************************* /
//    Creating a pow function, needs compile time Power as types are         /
//    generated                                                              /
// ************************************************************************* /
template <int power, class Units, class BaseType, class Tag>
constexpr auto pow(const Quantity<Units, BaseType, Tag>& a) noexcept {
  if constexpr (power == 1) {
    return a;
  } else if constexpr (power > 1) {
    return a * pow<power - 1>(a);
  } else if constexpr (power == -1) {
    return 1 / a;
  } else if constexpr (power < -1) {
    return (1 / a) * pow<power + 1>(a);
  } else if constexpr (power == 0) {
    return 1;
  }
}

// ************************************************************************* /
//    Creating a sqrt function, needs compile time Power as types are        /
//    generated                                                              /
// ************************************************************************* /

/*
 * Constexpr version of the square root
 * Return value:
 *   - For a finite and non-negative value of "x", returns an approximation for
 * the square root of "x"
 *   - Otherwise, returns NaN
 */
namespace Impl {
  template <class T> //, std::enable_if_t<std::is_arithmetic_v(T{})>>
  T constexpr sqrtNewtonRaphson(T x, T curr, T prev) {
    return curr == prev ? curr
                        : sqrtNewtonRaphson(x, (curr + x / curr) / 2, curr);
  }

  template <class T> //, std::enable_if_t<std::is_arithmetic_v(T{})>>
  T constexpr sqrt(T x) {
    assert(x > 0);
    return x >= 0 && x < std::numeric_limits<T>::infinity()
               ? Impl::sqrtNewtonRaphson(x, x, T{0})
               : std::numeric_limits<T>::quiet_NaN();
  }
} // namespace Impl

namespace std {
  template <class Units, class BaseType, class Tag>
  constexpr auto sqrt(const Quantity<Units, BaseType, Tag>& a) {
    using Units0 = units::derived_unity_t<decltype(Units{})>;
    using Units1 = decltype(units::sqrt(Units0{}));
    auto val = Impl::sqrt(a.underlying_value_no_prefix());
    return Quantity<Units1, BaseType, Tag>{val};
  }
} // namespace std

// ************************************************************************* /
//    Overloading the abs functions /
// ************************************************************************* /
namespace std {
  template <class Units, class BaseType, class Tag>
  constexpr Quantity<Units, BaseType, Tag>
  abs(const Quantity<Units, BaseType, Tag>& a) noexcept {
    return Quantity<Units, BaseType, Tag>{std::abs(a.underlying_value())};
  }

  template <class Units, class BaseType, class Tag,
            typename = std::enable_if_t<std::is_floating_point_v<BaseType>>>
  constexpr Quantity<Units, BaseType, Tag>
  fabs(const Quantity<Units, BaseType, Tag>& a) noexcept {
    return Quantity<Units, BaseType, Tag>{std::fabs(a.underlying_value())};
  }
} // namespace std

// ************************************************************************* /
//    Specialising the numeric_limits functions /
// ************************************************************************* /

namespace std {
  template <class Units, class BaseType, class Tag>
  class numeric_limits<Quantity<Units, BaseType, Tag>> {
  public:
    using nlim = std::numeric_limits<BaseType>;
    using Quant = Quantity<Units, BaseType, Tag>;

    constexpr static auto min() noexcept { return Quant{nlim::min()}; }
    constexpr static auto lowest() noexcept { return Quant{nlim::lowest()}; }
    constexpr static auto max() noexcept { return Quant{nlim::max()}; }
    constexpr static auto epsilon() noexcept { return Quant{nlim::epsilon()}; }
    constexpr static auto round_error() noexcept {
      return Quant{nlim::round_error()};
    }
    constexpr static auto infinity() noexcept {
      return Quant{nlim::infinity()};
    }
    constexpr static auto quiet_NaN() noexcept {
      return Quant{nlim::quiet_NaN()};
    }
    constexpr static auto signaling_NaN() noexcept {
      return Quant{nlim::signaling_NaN()};
    }
    constexpr static auto denorm_min() noexcept {
      return Quant{nlim::denorm_min()};
    }

    static constexpr auto max_exponent = nlim::max_exponent;
    static constexpr auto max_exponent10 = nlim::max_exponent10;
    static constexpr auto min_exponent = nlim::min_exponent;
    static constexpr auto min_exponent10 = nlim::min_exponent10;
    static constexpr auto radix = nlim::radix;
    static constexpr auto digits = nlim::digits;
    static constexpr auto max_digits10 = nlim::max_digits10;
    static constexpr auto is_specialized = nlim::is_specialized;
    static constexpr auto is_signed = nlim::is_signed;
    static constexpr auto is_integer = nlim::is_integer;
    static constexpr auto is_exact = nlim::is_exact;
    static constexpr auto has_infinity = nlim::has_infinity;
    static constexpr auto has_quiet_NaN = nlim::has_quiet_NaN;
    static constexpr auto has_signaling_NaN = nlim::has_signaling_NaN;
    static constexpr auto has_denorm = nlim::has_denorm;
    static constexpr auto has_denorm_loss = nlim::has_denorm_loss;
    static constexpr auto round_style = nlim::round_style;
    static constexpr auto is_iec559 = nlim::is_iec559;
    static constexpr auto is_bounded = nlim::is_bounded;
    static constexpr auto is_modulo = nlim::is_modulo;
    static constexpr auto traps = nlim::traps;
    static constexpr auto tinyness_before = nlim::tinyness_before;
  };
} // namespace std

// Helper functions, nicer to write huge(T{}) than numeric_limits<T>::()
// (borrowed from Fortan!)
template <class Units, class BaseType, class Tag>
constexpr auto huge(Quantity<Units, BaseType, Tag>) noexcept {
  return std::numeric_limits<Quantity<Units, BaseType, Tag>>::max();
}

template <class Units, class BaseType, class Tag>
constexpr auto tiny(Quantity<Units, BaseType, Tag>) noexcept {
  return std::numeric_limits<Quantity<Units, BaseType, Tag>>::min();
}

template <class Units, class BaseType, class Tag>
constexpr auto epsilon(Quantity<Units, BaseType, Tag>) noexcept {
  return std::numeric_limits<Quantity<Units, BaseType, Tag>>::epsilon();
}