#pragma once

#include "base_dimensions.hpp"
#include "common_units.hpp"
#include "derived_dimensions.hpp"
#include "prefixes.hpp"
#include "quantity.hpp"

// Dimensionless
using dimensionless = Quantity<units::derived_t<units::unity>>;

// Masses
using kg = Quantity<kg_t>;

// Lengths
using metres = Quantity<metres_t>;
using cm = Quantity<cm_t>;
using km = Quantity<km_t>;

// Areas
using metres2 = Quantity<metres2_t>;
using acre = Quantity<acre_t>;
using hectare = Quantity<hectare_t>;

// Volumes
using metres3 = Quantity<metres3_t>;
using litres = Quantity<litres_t>;
using us_gallon = Quantity<us_gallon_t>;
using imp_gallon = Quantity<imp_gallon_t>;

// Length Extras
using metres05 = Quantity<metres05_t>;

// Times
using seconds = Quantity<seconds_t>;
using minutes = Quantity<minutes_t>;
using hours = Quantity<hours_t>;
using days = Quantity<days_t>;

// Rates
using per_second = Quantity<per_second_t>;
using per_min = Quantity<per_min_t>;
using per_hour = Quantity<per_hour_t>;
using per_days = Quantity<per_days_t>;

// Time Extras
using seconds2 = Quantity<seconds2_t>;

// Length + Time
using metres_per_sec = Quantity<metres_per_sec_t>;
using metres_per_sec2 = Quantity<metres_per_sec2_t>;

// Volume + Time
using kg_per_sec = Quantity<decltype(kg_t{} / seconds_t{})>;
using m3_per_sec = Quantity<decltype(metres3_t{} / seconds_t{})>;

// Energy
using Joules = Quantity<Joules_t>;
using Watts = Quantity<Watts_t>;

// Momentum
using kg_metres_per_sec = Quantity<kg_metres_per_sec_t>;
using kg_metres_per_sec2 = Quantity<kg_metres_per_sec2_t>;

// Engineering units
using Newtons = Quantity<Newtons_t>;
using Pascals = Quantity<Pascals_t>;
using MPam05 = Quantity<MPam05_t>;
