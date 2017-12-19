#pragma once

#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "prefixes.hpp"

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
template <class Units, class Ratio = si::unity, class BaseType = double,
          class Tag = std::false_type>
class Quantity {
  BaseType _val;

public:
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

template <class Units, class Ratio0, class Ratio1, class BaseType, class Tag>
constexpr auto operator+(const Quantity<Units, Ratio0, BaseType, Tag>& a,
                         const Quantity<Units, Ratio1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  auto tmp{aa};
  return tmp += bb;
}

template <class Units, class Ratio0, class Ratio1, class BaseType, class Tag>
constexpr auto operator-(const Quantity<Units, Ratio0, BaseType, Tag>& a,
                         const Quantity<Units, Ratio1, BaseType, Tag>& b) {
  const auto & [ aa, bb ] = rescale(a, b);
  auto tmp{aa};
  return tmp -= bb;
}

template <class Units, class Ratio, class BaseType, class Tag, class Div,
          typename = std::enable_if_t<valid_summand(Div{})>>
constexpr auto operator/(const Quantity<Units, Ratio, BaseType, Tag>& a,
                         const Div& b) {
  return Quantity<Units, Ratio, BaseType, Tag>{a.underlying_value() / b};
}

template <class Units, class Ratio, class BaseType, class Tag, class Mult,
          typename = std::enable_if_t<valid_summand(Mult{})>>
constexpr auto operator*(const Quantity<Units, Ratio, BaseType, Tag>& a,
                         const Mult& b) {
  return Quantity<Units, Ratio, BaseType, Tag>{a.underlying_value() * b};
}

template <class Units, class Ratio, class BaseType, class Tag, class Mult,
          typename = std::enable_if_t<valid_summand(Mult{})>>
constexpr auto operator*(const Mult& b,
                         const Quantity<Units, Ratio, BaseType, Tag>& a) {
  return Quantity<Units, Ratio, BaseType, Tag>{a.underlying_value() * b};
}

template <class Units0, class Units1, class Ratio0, class Ratio1,
          class BaseType, class Tag>
constexpr auto operator*(const Quantity<Units0, Ratio0, BaseType, Tag>& a,
                         const Quantity<Units1, Ratio1, BaseType, Tag>& b) {
  //using Units = decltype(si::Impl::multiply(Units0{}, Units1{}));
  using Units = decltype(Units0{} * Units1{});
  if constexpr (std::is_same_v<Ratio0, Ratio1>) {
    return Quantity<Units, Ratio0, BaseType, Tag>{a.underlying_value() *
                                                  b.underlying_value()};
  } else if constexpr (Ratio0::num == 1 && Ratio0::den == 1) {
    using Ratio2 = std::ratio_divide<Ratio0, Ratio1>;
    return Quantity<Units, Ratio0, BaseType, Tag>{a.underlying_value() *
                                                  b.underlying_value() *
                                                  Ratio2::num / Ratio2::den};
  } else if constexpr (Ratio1::num == 1 && Ratio1::den == 1) {
    using Ratio2 = std::ratio_divide<Ratio1, Ratio0>;
    return Quantity<Units, Ratio1, BaseType, Tag>{a.underlying_value() *
                                                  b.underlying_value() *
                                                  Ratio2::num / Ratio2::den};
  } else {
    using Ratio2 = std::ratio_divide<Ratio0, Ratio1>;
    return Quantity<Units, Ratio0, BaseType, Tag>{a.underlying_value() *
                                                  b.underlying_value() *
                                                  Ratio2::num / Ratio2::den};
  }
}
