#pragma once

#include <boost/hana.hpp>
#include <ratio>
//#include <boost/hana/experimental/printable.hpp>
#include <utility>

namespace hana = boost::hana;

template <class, class, class>
class Quantity;

template <class Units0, class Units1, class BaseType, class Tag>
constexpr auto rescale(const Quantity<Units0, BaseType, Tag>& a,
                       const Quantity<Units1, BaseType, Tag>& b) {
  static_assert(same_dimension(Units0{}, Units1{}));
  using T0 = Quantity<Units0, BaseType, Tag>;
  using T1 = Quantity<Units1, BaseType, Tag>;
  using Ratio0 = typename Units0::prefix;
  using Ratio1 = typename Units1::prefix;

  if constexpr (std::is_same_v<Ratio0, Ratio1>) {
    return std::tuple<const T0&, const T1&>{a, b};
  } else if constexpr (Ratio0::num == 1 && Ratio0::den == 1) {
    return std::tuple<const T0&, T0>{a, T0{b.underlying_value_no_prefix()}};
  } else if constexpr (Ratio1::num == 1 && Ratio1::den == 1) {
    return std::tuple<T1, const T1&>{T1{a.underlying_value_no_prefix()}, b};
  } else {
    using Ratio2 = std::ratio_divide<Ratio1, Ratio0>;
    return std::tuple<const T0&, T0>{a, T0{b.underlying_value()} * Ratio2::num /
                                            Ratio2::den};
  }
}

namespace units {
  using nano = std::nano;
  using milli = std::milli;
  using centi = std::centi;
  using unity = std::ratio<1, 1>;
  using kilo = std::kilo;
  using mega = std::mega;
  using giga = std::giga;
} // namespace units
