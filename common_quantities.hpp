#pragma once
#include "base_dimensions.hpp"
#include "common_units.hpp"
#include "derived_dimensions.hpp"
#include "prefixes.hpp"
#include "quantity.hpp"

using metre = Quantity<m_t>;
using km = Quantity<km_t>;
using Pa = Quantity<Pa_t>;
using MPam05 = Quantity<MPam05_t>;
// using cm = si::Length<1, 1, std::ratio_multiply<decltype(m::prefix),
// si::centi>>;  using km = si::Length<1, 1,
// std::ratio_multiply<decltype(m::prefix), si::kilo>>;

// using kg = si::Mass<1, 1, si::unity>;
