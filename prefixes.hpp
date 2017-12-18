#pragma once

#include <boost/hana.hpp>
#include <ratio>
//#include <boost/hana/experimental/printable.hpp>
#include <utility>

namespace hana = boost::hana;

template <class, class, class, class>
class Quantity;

template <class Units, class Ratio0, class Ratio1, class BaseType, class Tag>
constexpr auto rescale(const Quantity<Units, Ratio0, BaseType, Tag>& a,
                       const Quantity<Units, Ratio1, BaseType, Tag>& b) {
  using T0 = Quantity<Units, Ratio0, BaseType, Tag>;
  using T1 = Quantity<Units, Ratio1, BaseType, Tag>;
  if constexpr (std::is_same_v<Ratio0, Ratio1>) {
    return std::tuple<const T0&, const T1&>{a, b};
  } else if constexpr (Ratio0::num == 1 && Ratio0::den == 1) {
    using Ratio2 = std::ratio_divide<Ratio0, Ratio1>;
    return std::tuple<const T0&, T0>{a, T0{b.underlying_value()} * Ratio2::num /
                                            Ratio2::den};
  } else if constexpr (Ratio1::num == 1 && Ratio1::den == 1) {
    using Ratio2 = std::ratio_divide<Ratio1, Ratio0>;
    return std::tuple<T1, const T1&>{
        T1{a.underlying_value()} * Ratio2::num / Ratio2::den, b};
  } else {
    using Ratio2 = std::ratio_divide<Ratio0, Ratio1>;
    return std::tuple<const T0&, T0>{a, T0{b.underlying_value()} * Ratio2::num /
                                            Ratio2::den};
  }
}

namespace si {
  using nano = std::nano;
  using milli = std::milli;
  using centi = std::centi;
  using unity = std::ratio<1, 1>;
  using kilo = std::kilo;
  using mega = std::mega;
  using giga = std::giga;
} // namespace si
