#pragma once

#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "derived_dimensions_printing.hpp"
#include "prefixes.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

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
template <class Obj>
constexpr auto is_quantity(Obj) {
  return std::false_type{};
}

template <class Units, class BaseType, class Tag>
constexpr auto is_quantity(Quantity<Units, BaseType, Tag>) {
  return std::true_type{};
}

template <class Units, class BaseType_ = double, class Tag_ = std::false_type>
class Quantity {
public:
  using Prefix = typename Units::prefix;
  using Tag = Tag_;
  using BaseType = BaseType_;
  constexpr Quantity() = default;

  // Extra ctor to stop narrowing warning errors on creation, these can occur
  // when an int is passed to the BaseType ctor when the basetype is double.
  template <class U, std::enable_if_t<std::is_arithmetic_v<U>>>
  constexpr Quantity(U v) noexcept : _val(std::move(v)) {}

  constexpr Quantity(const BaseType& v) noexcept(
      std::is_nothrow_copy_constructible_v<BaseType>)
      : _val(v) {}
  constexpr Quantity(BaseType&& v) noexcept(
      std::is_nothrow_move_constructible_v<BaseType>)
      : _val(std::move(v)) {}
  constexpr Quantity(Quantity&& o) noexcept(
      std::is_nothrow_move_constructible_v<BaseType>) = default;
  constexpr Quantity(const Quantity& o) noexcept(
      std::is_nothrow_copy_constructible_v<BaseType>) = default;
  constexpr Quantity& operator=(Quantity&& o) noexcept(
      std::is_nothrow_move_assignable_v<BaseType>) = default;
  constexpr Quantity& operator=(const Quantity& o) noexcept(
      std::is_nothrow_copy_assignable_v<BaseType>) = default;

  // Casts to basetype, doesn't convert to SI etc.
  // Return reference to BaseType if Quantity is an lvalue (& qualified)
  // Return value of BaseType if Quantity is an rvalue (&& qualified)
  constexpr const BaseType& underlying_value() const& noexcept { return _val; }
  constexpr BaseType& underlying_value() & noexcept { return _val; }
  constexpr BaseType underlying_value() const&& noexcept { return _val; }
  constexpr BaseType underlying_value() && noexcept { return _val; }

  constexpr explicit operator BaseType&() const& noexcept { return _val; }
  constexpr explicit operator BaseType&() & noexcept { return _val; }
  constexpr explicit operator BaseType() const&& noexcept { return _val; }
  constexpr explicit operator BaseType() && noexcept { return _val; }

  /// Returns a copy of _val, converted to a prefix of 1, so if the units of
  /// this type are km and the _val is 1, then this returns 1000 (in m)
  constexpr BaseType underlying_value_no_prefix() const noexcept {
    return _val * Prefix::num / Prefix::den;
  }

  // Only want to define this if BaseType is not bool, otherwise the casts to
  // BaseType would already define the bool operator
  template <class Proxy = BaseType,
            typename = std::enable_if_t<!std::is_same_v<Proxy, bool>>>
  constexpr explicit operator bool() const noexcept {
    return static_cast<bool>(_val);
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

  template <class Div, typename = std::enable_if_t<std::is_arithmetic_v<Div>>>
  Quantity& operator/=(const Div& d) noexcept {
    _val /= d;
    return *this;
  }

  Quantity operator-() const noexcept { return Quantity{-_val}; }

  template <class Mult, typename = std::enable_if_t<std::is_arithmetic_v<Mult>>>
  Quantity& operator*=(const Mult& d) noexcept {
    _val *= d;
    return *this;
  }

  template <class Units1,
            typename = std::enable_if_t<is_dimensionless(Units1{})>>
  Quantity& operator/=(const Quantity<Units1, BaseType, Tag>& d) noexcept {
    using Ratio1 = typename Units1::prefix;
    using Ratio2 = std::ratio_divide<Ratio1, Prefix>;
    _val /= (d.underlying_value() * Ratio2::num / Ratio2::den);
    return *this;
  }

  template <class Units1,
            typename = std::enable_if_t<is_dimensionless(Units1{})>>
  Quantity& operator*=(const Quantity<Units1, BaseType, Tag>& d) noexcept {
    using Ratio1 = typename Units1::prefix;
    using Ratio2 = std::ratio_divide<Ratio1, Prefix>;
    _val *= d.underlying_value() * Ratio2::num / Ratio2::den;
    return *this;
  }

private:
  BaseType _val;

  static_assert(!is_quantity(Units{}), "You've passed in a Quantity as the 1st "
                                       "template parameter, maybe missing a "
                                       "'_t' in the type");
  static_assert(units::is_dimensions(Units{}));
  static_assert(!units::is_ratio(BaseType{}), "pass in the underlying type as "
                                              "the 2nd argument, did you mean "
                                              "to change the Dimensions type?");
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

// ********************

// ************************************************************************* /
//    Comparison operators, requires the Quantities to have the same         /
//    dimensions, converts the Quantities to the same prefix.                /
//    /
// ************************************************************************* /

template <class... Args>
struct Names {
  operator bool() const = delete;
};

template <class Dimension>
constexpr auto make_names_from_dimension([[maybe_unused]] Dimension) {
  using namespace units;
  return Names<
      Length<Dimension::length::num, Dimension::length::den>,
      Mass<Dimension::mass::num, Dimension::mass::den>,
      Time<Dimension::time::num, Dimension::time::den>,
      Current<Dimension::current::num, Dimension::current::den>,
      Temperature<Dimension::temperature::num, Dimension::temperature::den>,
      Amount<Dimension::amount::num, Dimension::amount::den>,
      Luminosity<Dimension::luminosity::num, Dimension::luminosity::den>>{};
}

template <bool DimensionsBalance, class lhs, class rhs>
constexpr auto use_dimension_names() {
  static_assert(DimensionsBalance);
}

/// Equality
template <class Units0, class Units1, class BaseType, class Tag0, class Tag1>
constexpr auto operator==(const Quantity<Units0, BaseType, Tag0>& a,
                          const Quantity<Units1, BaseType, Tag1>& b) {
  use_dimension_names<same_dimension(Units0{}, Units1{}),
                      decltype(make_names_from_dimension(Units0{})),
                      decltype(make_names_from_dimension(Units1{}))>();
  static_assert(std::is_same_v<Tag0, Tag1>);
  const auto& [aa, bb] = rescale(a, b);
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
template <class Units0, class Units1, class BaseType, class Tag0, class Tag1,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator<(const Quantity<Units0, BaseType, Tag0>& a,
                         const Quantity<Units1, BaseType, Tag1>& b) {
  use_dimension_names<same_dimension(Units0{}, Units1{}),
                      decltype(make_names_from_dimension(Units0{})),
                      decltype(make_names_from_dimension(Units1{}))>();
  static_assert(std::is_same_v<Tag0, Tag1>);
  const auto& [aa, bb] = rescale(a, b);
  return aa.underlying_value() < bb.underlying_value();
}

/// Less than or equal to
template <class Units0, class Units1, class BaseType, class Tag0, class Tag1,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator<=(const Quantity<Units0, BaseType, Tag0>& a,
                          const Quantity<Units1, BaseType, Tag1>& b) {
  use_dimension_names<same_dimension(Units0{}, Units1{}),
                      decltype(make_names_from_dimension(Units0{})),
                      decltype(make_names_from_dimension(Units1{}))>();
  static_assert(std::is_same_v<Tag0, Tag1>);
  const auto& [aa, bb] = rescale(a, b);
  return aa.underlying_value() <= bb.underlying_value();
}

/// Greater than
template <class Units0, class Units1, class BaseType, class Tag0, class Tag1,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator>(const Quantity<Units0, BaseType, Tag0>& a,
                         const Quantity<Units1, BaseType, Tag1>& b) {
  use_dimension_names<same_dimension(Units0{}, Units1{}),
                      decltype(make_names_from_dimension(Units0{})),
                      decltype(make_names_from_dimension(Units1{}))>();
  static_assert(std::is_same_v<Tag0, Tag1>);
  const auto& [aa, bb] = rescale(a, b);
  return aa.underlying_value() > bb.underlying_value();
}

/// Greater than or equal
template <class Units0, class Units1, class BaseType, class Tag0, class Tag1>
constexpr auto operator>=(const Quantity<Units0, BaseType, Tag0>& a,
                          const Quantity<Units1, BaseType, Tag1>& b) {
  use_dimension_names<same_dimension(Units0{}, Units1{}),
                      decltype(make_names_from_dimension(Units0{})),
                      decltype(make_names_from_dimension(Units1{}))>();
  static_assert(std::is_same_v<Tag0, Tag1>);

  const auto& [aa, bb] = rescale(a, b);
  return aa.underlying_value() >= bb.underlying_value();
}

/// Within a tolerance
template <class Units0, class Units1, class Units2, class BaseType, class Tag0,
          class Tag1, class Tag2>
constexpr auto within(const Quantity<Units0, BaseType, Tag0>& a,
                      const Quantity<Units1, BaseType, Tag1>& b,
                      const Quantity<Units2, BaseType, Tag2>& tol) {
  use_dimension_names<same_dimension(Units0{}, Units1{}),
                      decltype(make_names_from_dimension(Units0{})),
                      decltype(make_names_from_dimension(Units1{}))>();
  use_dimension_names<same_dimension(Units0{}, Units2{}),
                      decltype(make_names_from_dimension(Units0{})),
                      decltype(make_names_from_dimension(Units2{}))>();

  static_assert(std::is_same_v<Tag0, Tag1>);
  static_assert(std::is_same_v<Tag0, Tag2>);

  assert(tol.underlying_value() >= 0);
  const auto delta = a - b;
  using Delta = decltype(delta);
  return Delta{std::abs(delta.underlying_value())} <= tol;
}

// ************************************************************************* /
//    Addition and subtraction operators                                     /
// ************************************************************************* /

/// Addition
template <class Units0, class Units1, class BaseType, class Tag0, class Tag1>
constexpr auto operator+(const Quantity<Units0, BaseType, Tag0>& a,
                         const Quantity<Units1, BaseType, Tag1>& b) {
  use_dimension_names<same_dimension(Units0{}, Units1{}),
                      decltype(make_names_from_dimension(Units0{})),
                      decltype(make_names_from_dimension(Units1{}))>();
  static_assert(std::is_same_v<Tag0, Tag1>);

  const auto& [aa, bb] = rescale(a, b);
  auto tmp{aa};
  return tmp += bb;
}

/// Subtraction
template <class Units0, class Units1, class BaseType, class Tag0, class Tag1>
constexpr auto operator-(const Quantity<Units0, BaseType, Tag0>& a,
                         const Quantity<Units1, BaseType, Tag1>& b) {
  use_dimension_names<same_dimension(Units0{}, Units1{}),
                      decltype(make_names_from_dimension(Units0{})),
                      decltype(make_names_from_dimension(Units1{}))>();
  static_assert(std::is_same_v<Tag0, Tag1>);

  const auto& [aa, bb] = rescale(a, b);
  auto tmp{aa};
  return tmp -= bb;
}

// ************************************************************************* /
//    Division and multiplication operators                                  /
// ************************************************************************* /

/// Division, creates a new Quantity Type with the correct dimensions and a
/// prefix of unity (1). For example:
///
template <class Units0, class Units1, class BaseType, class Tag0, class Tag1>
constexpr auto operator/(const Quantity<Units0, BaseType, Tag0>& a,
                         const Quantity<Units1, BaseType, Tag1>& b) {
  static_assert(tags_compatible_multiplication<Tag0, Tag1>());
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
template <class Units0, class Units1, class BaseType, class Tag0, class Tag1>
constexpr auto operator*(const Quantity<Units0, BaseType, Tag0>& a,
                         const Quantity<Units1, BaseType, Tag1>& b) {
  static_assert(tags_compatible_multiplication<Tag0, Tag1>());
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
          typename = std::enable_if_t<std::is_arithmetic_v<Div>>>
constexpr auto operator/(const Quantity<Units, BaseType, Tag>& a,
                         const Div& b) {
  return Quantity<Units, BaseType, Tag>{a.underlying_value() / b};
}

template <class Units, class BaseType, class Tag, class Div,
          typename = std::enable_if_t<std::is_integral_v<Div>>>
constexpr auto operator/(const Div& b,
                         const Quantity<Units, BaseType, Tag>& a) {
  using InvUnits = decltype(invert(Units{}));
  return Quantity<InvUnits, BaseType, Tag>{b / a.underlying_value()};
}

template <class Units, class BaseType, class Tag, class Mult,
          typename = std::enable_if_t<std::is_arithmetic_v<Mult>>>
constexpr auto operator*(const Quantity<Units, BaseType, Tag>& a,
                         const Mult& b) {
  return Quantity<Units, BaseType, Tag>{a.underlying_value() * b};
}

template <class Units, class BaseType, class Tag, class Mult,
          typename = std::enable_if_t<std::is_arithmetic_v<Mult>>>
constexpr auto operator*(const Mult& b,
                         const Quantity<Units, BaseType, Tag>& a) {
  return Quantity<Units, BaseType, Tag>{a.underlying_value() * b};
}

// ************************************************************************* /
//    Comparison Functions, Dimensionless Quantity on LHS                    /
// ************************************************************************* /

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator>(const Quantity<Units, BaseType, Tag>& a,
                         const Rhs& comp) {
  return a.underlying_value_no_prefix() > comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator>=(const Quantity<Units, BaseType, Tag>& a,
                          const Rhs& comp) {
  return a.underlying_value_no_prefix() >= comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator<(const Quantity<Units, BaseType, Tag>& a,
                         const Rhs& comp) {
  return a.underlying_value_no_prefix() < comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator<=(const Quantity<Units, BaseType, Tag>& a,
                          const Rhs& comp) {
  return a.underlying_value_no_prefix() <= comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator==(const Quantity<Units, BaseType, Tag>& a,
                          const Rhs& comp) {
  return a.underlying_value_no_prefix() == comp;
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator!=(const Quantity<Units, BaseType, Tag>& a,
                          const Rhs& comp) {
  return !(a == comp);
}

// ************************************************************************* /
//    Comparison Functions, Dimensionless Quantity on RHS                    /
// ************************************************************************* /
// useful for ratios, e.g. length1/ length0 > 5
template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator>(const Rhs& comp,
                         const Quantity<Units, BaseType, Tag>& a) {
  return comp > a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator>=(const Rhs& comp,
                          const Quantity<Units, BaseType, Tag>& a) {
  return comp >= a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator<(const Rhs& comp,
                         const Quantity<Units, BaseType, Tag>& a) {
  return comp < a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator<=(const Rhs& comp,
                          const Quantity<Units, BaseType, Tag>& a) {
  return comp <= a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator==(const Rhs& comp,
                          const Quantity<Units, BaseType, Tag>& a) {
  return comp == a.underlying_value_no_prefix();
}

template <class Units, class BaseType, class Tag, class Rhs,
          typename = std::enable_if_t<std::is_arithmetic_v<Rhs>>,
          typename = std::enable_if_t<is_dimensionless(Units{})>>
constexpr bool operator!=(const Rhs& comp,
                          const Quantity<Units, BaseType, Tag>& a) {
  return !(comp == a);
}
