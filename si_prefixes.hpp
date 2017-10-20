#pragma once

#include <ratio>
#include <boost/hana.hpp>
//#include <boost/hana/experimental/printable.hpp>
#include <utility>
 
 
namespace hana = boost::hana;
 
template<class, class, class, class>
struct Quantity;
 
template<class Tag, class Units, class Ratio0, class Ratio1, class BaseType>
constexpr auto rescale(const Quantity<Tag, Units, Ratio0, BaseType>& a, const Quantity<Tag, Units, Ratio1, BaseType>& b) {
  using T0 = Quantity<Tag, Units, Ratio0, BaseType>;
  using T1 = Quantity<Tag, Units, Ratio1, BaseType>;
   if constexpr (std::is_same_v<Ratio0, Ratio1>) {
     return std::tuple(a, b);
   } else if constexpr (Ratio0::num == 1 && Ratio0::den == 1) {
     using Ratio2 = std::ratio_divide<Ratio0, Ratio1>;
     return std::tuple<const T0&, T0>{a, T0{b._val} * Ratio2::num / Ratio2::den};
   } else if constexpr (Ratio1::num == 1 && Ratio1::den == 1) {
     using Ratio2 = std::ratio_divide<Ratio1, Ratio0>;
     return std::tuple<T1, const T1&>{T1{a._val} * Ratio2::num / Ratio2::den, b};
   } else {
     using Ratio2 = std::ratio_divide<Ratio0, Ratio1>;
     return std::tuple<const T0&, T0>{a, T0{b._val} * Ratio2::num / Ratio2::den};
   }
}
 
 
