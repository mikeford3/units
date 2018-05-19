#pragma once

#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "derived_dimensions_printing.hpp"
#include "prefixes.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

template <class Arg>
constexpr bool valid_summand([[maybe_unused]] Arg arg = Arg{}) {
  if constexpr (std::is_floating_point_v<Arg> || std::is_integral_v<Arg>) {
    return true;
  }
  return false;
}

/*!
 * \brief Allow multiplication/division for identical tags, or if only one real
 * tag
 *
 * This is to allow a quantity to be multiplied by time say.
 * e.g. allows Quantity<...,...,Proxy> * Quantity<...,...,Proxy>,
 *             Quantity<...,...,Proxy> * Quantity<...,...,std::false_type>
 *             Quantity<...,...,std::false_type> * Quantity<...,...,Proxy>
 * */
template <class Tag0, class Tag1>
constexpr bool tags_compatible_multiplication() {
  if constexpr (std::is_same_v<Tag0, Tag1>) {
    return true;
  } else if constexpr (std::is_same_v<Tag0, std::false_type>) {
    return true;
  } else if constexpr (std::is_same_v<Tag1, std::false_type>) {
    return true;
  } else {
    return false;
  }
}

/*!
 * \brief A class to give values units and tags.
 *
 * \tparam Units A derived_t class that provides the units for this quantity.
 * \tparam BaseType The underlying type, defaults to double.
 * \tparam Tag A proxy to allow Quantities with the same dimensions to be
 * distinguished.
 *
 * Only adds or substracts when Units and Tag are the same.
 *
 * Can be divided and multiplied by floats and integers to scale it.
 *
 * Returns the correct type for multiplications and divisions with other
 * Quantities, which are defined as free functions. The operators *= and
 * /= are only defined for floats, doubles and dimensionless Quantities to
 * allow for scaling, they make no sense for other quantities, (what is
 * 1 metre /= 2 kg)?
 *
 * If dimensionless it can be compared to floats and intergers to allow
 * comparisons of ratios, for example:
 *  Quantity<A, B, C> / Quantity<A, B, C> returns a dimensionless Quantity,
 * which can then be compared to floats and integers. These are defeined as
 * free functions.
 */

template <class Units, class BaseType_ = double, class Tag_ = std::false_type>
class Quantity {
public:
  using Prefix = typename Units::prefix;
  using Tag = Tag_;
  using BaseType = BaseType_;
  constexpr explicit Quantity(const BaseType& v) : _val(v) {}
  constexpr explicit Quantity(BaseType&& v) : _val(std::move(v)) {}
  constexpr Quantity(Quantity&& o) = default;
  constexpr Quantity(const Quantity& o) = default;
  constexpr Quantity& operator=(Quantity&& o) = default;
  constexpr Quantity& operator=(const Quantity& o) = default;

  constexpr const BaseType& underlying_value() const noexcept { return _val; }
  constexpr BaseType& underlying_value() noexcept { return _val; }

  /// Returns a copy of _val, converted to a prefix of 1, so if the units of
  /// this type are km and the _val is 1, then this returns 1000 (in m)
  constexpr BaseType underlying_value_no_prefix() const noexcept {
    return _val * Prefix::num / Prefix::den;
  }

  template <class Units1,
            typename = std::enable_if_t<same_dimension(Units{}, Units1{})>>
  Quantity& operator+=(const Quantity<Units1, BaseType, Tag>& o) noexcept {
    using Ratio1 = typename Units1::prefix;
    using Ratio2 = std::ratio_divide<Ratio1, Prefix>;
    _val += o.underlying_value() * Ratio2::num / Ratio2::den;
    return *this;
  }

  template <class Units1,
            typename = std::enable_if_t<same_dimension(Units{}, Units1{})>>
  Quantity& operator-=(const Quantity<Units1, BaseType, Tag>& o) noexcept {
    using Ratio1 = typename Units1::prefix;
    using Ratio2 = std::ratio_divide<Ratio1, Prefix>;
    _val -= o.underlying_value() * Ratio2::num / Ratio2::den;
    return *this;
  }

  template <class Div, typename = std::enable_if_t<valid_summand(Div{})>>
  Quantity& operator/=(const Div& d) {
    _val /= d;
    return *this;
  }

  template <class Mult, typename = std::enable_if_t<valid_summand(Mult{})>>
  Quantity& operator*=(const Mult& d) {
    _val *= d;
    return *this;
  }

  template <class Units1,
            typename = std::enable_if_t<is_dimensionless(Units1{})>>
  Quantity& operator/=(const Quantity<Units1, BaseType, Tag>& d) {
    using Ratio1 = typename Units1::prefix;
    using Ratio2 = std::ratio_divide<Ratio1, Prefix>;
    _val /= (d.underlying_value() * Ratio2::num / Ratio2::den);
    return *this;
  }

  template <class Units1,
            typename = std::enable_if_t<is_dimensionless(Units1{})>>
  Quantity& operator*=(const Quantity<Units1, BaseType, Tag>& d) {
    using Ratio1 = typename Units1::prefix;
    using Ratio2 = std::ratio_divide<Ratio1, Prefix>;
    _val *= d.underlying_value() * Ratio2::num / Ratio2::den;
    return *this;
  }

private:
  BaseType _val;
  static_assert(units::is_dimensions(Units{}));
  static_assert(!units::is_ratio(BaseType{}),
                "pass in the underlying type as the "
                "2nd argument, did you mean to "
                "change the Dimensions type?");
};

// ************************************************************************* /
//    Printing the Quantity                                                  /
// ************************************************************************* /
template <class Units, class BaseType, class Tag>
std::ostream& operator<<(std::ostream& os,
                         const Quantity<Units, BaseType, Tag>& q) {
  auto u = Units{};
  os << q.underlying_value() << " " << u;
  return os;
}

// ************************************************************************* /
//    Overloading the abs functions /
// ************************************************************************* /
namespace std {
  template <class Units, class BaseType, class Tag>
  constexpr Quantity<Units, BaseType, Tag>
  abs(const Quantity<Units, BaseType, Tag>& a) {
    return Quantity<Units, BaseType, Tag>{std::abs(a.underlying_value())};
  }
} // namespace std

// ************************************************************************* /
//    Creating a pow function, needs compile time Power as types are         /
//    generated                                                              /
// ************************************************************************* /
template <int power, class Units, class BaseType, class Tag>
constexpr auto pow(const Quantity<Units, BaseType, Tag>& a) {
  // static_assert(power >= 1);
  using Units0 = units::derived_unity_t<decltype(Units{})>;
  if constexpr (power == 1) {
    return a;
  } else {
    auto val = a.underlying_value_no_prefix();
    return Quantity<Units0, BaseType, Tag>{val} * pow<power - 1>(a);
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
  template <class T> //, std::enable_if_t<valid_summand(T{})>>
  T constexpr sqrtNewtonRaphson(T x, T curr, T prev) {
    return curr == prev ? curr
                        : sqrtNewtonRaphson(x, (curr + x / curr) / 2, curr);
  }

  template <class T> //, std::enable_if_t<valid_summand(T{})>>
  T constexpr sqrt(T x) {
    assert(x > 0);
    return x >= 0 && x < std::numeric_limits<T>::infinity()
               ? Impl::sqrtNewtonRaphson(x, x, T{0})
               : std::numeric_limits<T>::quiet_NaN();
  }
} // namespace Impl

template <class Units, class BaseType, class Tag>
constexpr auto sqrt(const Quantity<Units, BaseType, Tag>& a) {
  using Units0 = units::derived_unity_t<decltype(Units{})>;
  using Units1 = decltype(units::sqrt(Units0{}));
  auto val = Impl::sqrt(a.underlying_value_no_prefix());
  return Quantity<Units1, BaseType, Tag>{val};
}

// ************************************************************************* /
//    Comparison operators, requires the Quantities to have the same         /
//    dimensions, converts the Quantities to the same prefix.                /
//    /
// ************************************************************************* /

/// Equality
template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator==(const Quantity<Units0, BaseType, Tag>& a,
                          const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() == bb.underlying_value();
}

/// Inequality
template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator!=(const Quantity<Units0, BaseType, Tag>& a,
                          const Quantity<Units1, BaseType, Tag>& b) {
  return !(a == b);
}

/// Less than
template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator<(const Quantity<Units0, BaseType, Tag>& a,
                         const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() < bb.underlying_value();
}

/// Less than or equal to
template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator<=(const Quantity<Units0, BaseType, Tag>& a,
                          const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() <= bb.underlying_value();
}

/// Greater than
template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator>(const Quantity<Units0, BaseType, Tag>& a,
                         const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() > bb.underlying_value();
}

/// Greater than or equal
template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator>=(const Quantity<Units0, BaseType, Tag>& a,
                          const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() >= bb.underlying_value();
}

// ************************************************************************* /
//    Addition and subtraction operators                                     /
// ************************************************************************* /

/// Addition
template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator+(const Quantity<Units0, BaseType, Tag>& a,
                         const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  auto tmp{aa};
  return tmp += bb;
}

/// Subtraction
template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator-(const Quantity<Units0, BaseType, Tag>& a,
                         const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  auto tmp{aa};
  return tmp -= bb;
}

// ************************************************************************* /
//    Division and multiplication operators                                  /
// ************************************************************************* /

/// Division, creates a new Quantity Type with the correct dimensions and a
/// prefix of unity (1). For example:
///
template <
    class Units0, class Units1, class BaseType, class Tag0, class Tag1,
    typename = std::enable_if_t<tags_compatible_multiplication<Tag0, Tag1>()>>
constexpr auto operator/(const Quantity<Units0, BaseType, Tag0>& a,
                         const Quantity<Units1, BaseType, Tag1>& b) {
  using Units = units::derived_unity_t<decltype(Units0{} / Units1{})>;
  auto a_val = a.underlying_value_no_prefix();
  auto b_val = b.underlying_value_no_prefix();
  if constexpr (std::is_same_v<Tag0, std::false_type>) {
    return Quantity<Units, BaseType, Tag1>{a_val / b_val};
  } else {
    return Quantity<Units, BaseType, Tag0>{a_val / b_val};
  }
}

// convert types bases dimensions to their multiple, then return
// a variable with a prefix of unity
// e.g. 1 km * 2 km  ->  1,000 * 2,000 m^2 = 2,000,000 m^2
template <
    class Units0, class Units1, class BaseType, class Tag0, class Tag1,
    typename = std::enable_if_t<tags_compatible_multiplication<Tag0, Tag1>()>>
constexpr auto operator*(const Quantity<Units0, BaseType, Tag0>& a,
                         const Quantity<Units1, BaseType, Tag1>& b) {
  using Units = units::derived_unity_t<decltype(Units0{} * Units1{})>;
  auto a_val = a.underlying_value_no_prefix();
  auto b_val = b.underlying_value_no_prefix();
  if constexpr (std::is_same_v<Tag0, std::false_type>) {
    return Quantity<Units, BaseType, Tag1>{a_val * b_val};
  } else {
    return Quantity<Units, BaseType, Tag0>{a_val * b_val};
  }
}

// ************************************************************************* /
//    Multiply and Divide with with fundamental types                        /
// ************************************************************************* /

template <class Units, class BaseType, class Tag, class Div,
          typename = std::enable_if_t<valid_summand(Div{})>>
constexpr auto operator/(const Quantity<Units, BaseType, Tag>& a,
                         const Div& b) {
  return Quantity<Units, BaseType, Tag>{a.underlying_value() / b};
}

template <class Units, class BaseType, class Tag, class Mult,
          typename = std::enable_if_t<valid_summand(Mult{})>>
constexpr auto operator*(const Quantity<Units, BaseType, Tag>& a,
                         const Mult& b) {
  return Quantity<Units, BaseType, Tag>{a.underlying_value() * b};
}

template <class Units, class BaseType, class Tag, class Mult,
          typename = std::enable_if_t<valid_summand(Mult{})>>
constexpr auto operator*(const Mult& b,
                         const Quantity<Units, BaseType, Tag>& a) {
  return Quantity<Units, BaseType, Tag>{a.underlying_value() * b};
}

// ************************************************************************* /
//    Comparison Functions, Quantity on LHS                                  /
// ************************************************************************* /

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator>(const Quantity<Units, BaseType, Tag>& a,
                         const Rhs& comp) {
  return a.underlying_value_no_prefix() > comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator>=(const Quantity<Units, BaseType, Tag>& a,
                          const Rhs& comp) {
  return a.underlying_value_no_prefix() >= comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator<(const Quantity<Units, BaseType, Tag>& a,
                         const Rhs& comp) {
  return a.underlying_value_no_prefix() < comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator<=(const Quantity<Units, BaseType, Tag>& a,
                          const Rhs& comp) {
  return a.underlying_value_no_prefix() <= comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator==(const Quantity<Units, BaseType, Tag>& a,
                          const Rhs& comp) {
  return a.underlying_value_no_prefix() == comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator!=(const Quantity<Units, BaseType, Tag>& a,
                          const Rhs& comp) {
  return !(a == comp);
}

// ************************************************************************* /
//    Comparison Functions, Quantity on RHS                                  /
// ************************************************************************* /

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator>(const Rhs& comp,
                         const Quantity<Units, BaseType, Tag>& a) {
  return comp > a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator>=(const Rhs& comp,
                          const Quantity<Units, BaseType, Tag>& a) {
  return comp >= a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator<(const Rhs& comp,
                         const Quantity<Units, BaseType, Tag>& a) {
  return comp < a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator<=(const Rhs& comp,
                          const Quantity<Units, BaseType, Tag>& a) {
  return comp <= a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator==(const Rhs& comp,
                          const Quantity<Units, BaseType, Tag>& a) {
  return comp == a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<valid_summand(Rhs{})>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator!=(const Rhs& comp,
                          const Quantity<Units, BaseType, Tag>& a) {
  return !(comp == a);
}
