#pragma once

#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "derived_dimensions_printing.hpp"
#include "prefixes.hpp"

#include <cmath>
#include <iostream>

template <class Arg>
constexpr bool valid_summand([[maybe_unused]] Arg arg = Arg{}) {
  if constexpr (std::is_floating_point_v<Arg> || std::is_integral_v<Arg>) {
    return true;
  }
  return false;
}

/** Takes a derived unit class, maybe change to derived<Args...?>
 *  then optional parameters for the scale, an unterlying type and an option tag
 * to distinguish between Quantities with the same units but different types.*/
template <class Units, class BaseType = double, class Tag = std::false_type>
class Quantity {
  BaseType _val;
  static_assert(si::is_dimensions(Units{}));

public:
  using Prefix = typename Units::prefix;
  using BaseUnits = typename si::DimensionToBaseDimension<Units>::type;
  // normal ctors
  constexpr explicit Quantity(const BaseType& v) : _val(v) {}
  constexpr explicit Quantity(BaseType&& v) : _val(std::move(v)) {}
  // move/copy ctor
  constexpr Quantity(Quantity&& o) = default;
  constexpr Quantity(const Quantity& o) = default;
  constexpr Quantity& operator=(Quantity&& o) = default;
  constexpr Quantity& operator=(const Quantity& o) = default;

  constexpr const BaseType& underlying_value() const noexcept { return _val; }
  constexpr BaseType& underlying_value() noexcept { return _val; }

  Quantity& operator+=(const Quantity& o) noexcept {
    _val += o._val;
    return *this;
  }

  Quantity& operator-=(const Quantity& o) noexcept {
    _val -= o._val;
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
};

template <class Units, class BaseType, class Tag>
std::ostream& operator<<(std::ostream& os,
                         const Quantity<Units, BaseType, Tag>& q) {
  auto u = Units{};
  os << q.underlying_value() << " " << u;
  return os;
}

template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator==(const Quantity<Units0, BaseType, Tag>& a,
                          const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() == bb.underlying_value();
}

template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator!=(const Quantity<Units0, BaseType, Tag>& a,
                          const Quantity<Units1, BaseType, Tag>& b) {
  return !(a == b);
}

template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator<(const Quantity<Units0, BaseType, Tag>& a,
                         const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() < bb.underlying_value();
}

template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator<=(const Quantity<Units0, BaseType, Tag>& a,
                          const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() <= bb.underlying_value();
}

template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator>(const Quantity<Units0, BaseType, Tag>& a,
                         const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() > bb.underlying_value();
}

template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator>=(const Quantity<Units0, BaseType, Tag>& a,
                          const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  return aa.underlying_value() >= bb.underlying_value();
}

namespace std {
  template <class Units, class BaseType, class Tag>
  constexpr Quantity<Units, BaseType, Tag>
  abs(const Quantity<Units, BaseType, Tag>& a) {
    return Quantity<Units, BaseType, Tag>{std::abs(a.underlying_value())};
  }
} // namespace std

template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator+(const Quantity<Units0, BaseType, Tag>& a,
                         const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  auto tmp{aa};
  return tmp += bb;
}

template <class Units0, class Units1, class BaseType, class Tag,
          typename = std::enable_if_t<same_dimension(Units0{}, Units1{})>>
constexpr auto operator-(const Quantity<Units0, BaseType, Tag>& a,
                         const Quantity<Units1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  auto tmp{aa};
  return tmp -= bb;
}

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

//convert types bases dimensions to their multiple, then return 
//a variable with a prefix of unity
// e.g. 1 km * 2 km  ->  1,000 * 2,000 m^2 
template <class Units0, class Units1, class BaseType, class Tag>
constexpr auto operator*(const Quantity<Units0, BaseType, Tag>& a,
                         const Quantity<Units1, BaseType, Tag>& b) {
                           
  using Units = si::derived_unity_t<decltype(Units0{} * Units1{})>;
  using Ratio0 = typename Units0::prefix;
  using Ratio1 = typename Units1::prefix;
  auto a_val = a.underlying_value() * Ratio0::num / Ratio0::den;
  auto b_val = b.underlying_value() * Ratio1::num / Ratio1::den;
  return Quantity<Units, BaseType, Tag>{a_val * b_val};
  /*

  if constexpr (std::is_same_v<Ratio0, Ratio1>) {
    return Quantity<Units, BaseType, Tag>{a.underlying_value() *
                                          b.underlying_value()};
  } else if constexpr (Ratio0::num == 1 && Ratio0::den == 1) {
    using Ratio2 = std::ratio_divide<Ratio0, Ratio1>;
    return Quantity<Units, BaseType, Tag>{a.underlying_value() *
                                          b.underlying_value() * Ratio2::num /
                                          Ratio2::den};
  } else if constexpr (Ratio1::num == 1 && Ratio1::den == 1) {
    using Ratio2 = std::ratio_divide<Ratio1, Ratio0>;
    return Quantity<Units, BaseType, Tag>{a.underlying_value() *
                                          b.underlying_value() * Ratio2::num /
                                          Ratio2::den};
  } else {
    using Ratio2 = std::ratio_divide<Ratio0, Ratio1>;
    return Quantity<Units, BaseType, Tag>{a.underlying_value() *
                                          b.underlying_value() * Ratio2::num /
                                          Ratio2::den};
  }*/
}
