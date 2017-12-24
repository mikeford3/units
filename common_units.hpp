#pragma once
#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "prefixes.hpp"

using m = si::Length<1, 1, si::unity>;
using cm = si::Length<1, 1, std::ratio_multiply<decltype(m::prefix), si::centi>>;
using km = si::Length<1, 1, std::ratio_multiply<decltype(m::prefix), si::kilo>>;

using kg = si::Mass<1, 1, si::unity>;