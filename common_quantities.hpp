#pragma once
#include "base_dimensions.hpp"
#include "common_units.hpp"
#include "derived_dimensions.hpp"
#include "prefixes.hpp"
#include "quantity.hpp"

using metres = Quantity<m_t>;
using km = Quantity<km_t>;
using Pa = Quantity<Pa_t>;
using MPam05 = Quantity<MPam05_t>;
using m2 = Quantity<m2_t>;
using m3 = Quantity<m3_t>;
using m05 = Quantity<m05_t>;
// using cm = si::Length<1, 1, std::ratio_multiply<decltype(m::prefix),
// si::centi>>;  using km = si::Length<1, 1,
// std::ratio_multiply<decltype(m::prefix), si::kilo>>;

// using kg = si::Mass<1, 1, si::unity>;
