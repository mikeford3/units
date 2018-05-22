#pragma once
#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "prefixes.hpp"

// Masses
using kg_t = units::derived_t<units::Mass<1, 1, units::unity>>;

// Lengths
using metres_t = units::derived_t<units::Length<1, 1, units::unity>>;
using cm_t = units::derived_t<
    units::Length<1, 1, std::ratio_multiply<metres_t::prefix, units::centi>>>;
using km_t = units::derived_t<
    units::Length<1, 1, std::ratio_multiply<metres_t::prefix, units::kilo>>>;

// Areas
using metres2_t = units::derived_t<units::Length<2, 1, units::unity>>;
using acre_t = units::derived_t<
    metres2_t,
    std::ratio_multiply<metres2_t::prefix, std::ratio<404'685'642, 1>>>;
using hectare_t = units::derived_t<
    metres2_t, std::ratio_multiply<metres2_t::prefix, std::ratio<10'000, 1>>>;

// Volumes
using metres3_t = units::derived_t<units::Length<3>>;
using litres_t = units::derived_t<metres3_t, units::milli>;
using us_gallon_t =
    units::derived_t<litres_t, std::ratio<37'854'118, 10'000'000>>;
using imp_gallon_t = units::derived_t<litres_t, std::ratio<454'609, 100'000>>;

// Length Extras
using metres05_t = units::derived_t<units::Length<1, 2>>;

// Times
using seconds_t = units::derived_t<units::Time<1, 1, units::unity>>;
using minutes_t = units::derived_t<units::Time<
    1, 1, std::ratio_multiply<seconds_t::prefix, std::ratio<60, 1>>>>;
using hours_t = units::derived_t<units::Time<
    1, 1, std::ratio_multiply<seconds_t::prefix, std::ratio<3600, 1>>>>;
using days_t = units::derived_t<
    units::Time<1, 1, std::ratio_multiply<hours_t::prefix, std::ratio<24, 1>>>>;

// Rates
using per_second_t = units::derived_t<units::Time<-1, 1, units::unity>>;
using per_min_t = units::derived_t<units::Time<
    -1, 1, std::ratio_multiply<seconds_t::prefix, std::ratio<60, 1>>>>;
using per_hour_t = units::derived_t<units::Time<
    -1, 1, std::ratio_multiply<seconds_t::prefix, std::ratio<3600, 1>>>>;
using per_days_t = units::derived_t<units::Time<
    -1, 1, std::ratio_multiply<hours_t::prefix, std::ratio<24, 1>>>>;

// Time Extras
using seconds2_t = units::derived_t<units::Time<2, 1, units::unity>>;

// Length + Time
using velocity_t = units::derived_t<metres_t, units::Time<-1>>;
using acceleration_t = units::derived_t<velocity_t, units::Time<-1>>;

// Energy
using joules_t = decltype(kg_t{} / (metres2_t{} * seconds2_t{}));
using watts_t = decltype(joules_t{} / seconds_t{});

// Engineering units
using Newtons_t = units::derived_t<kg_t, acceleration_t>;
using Pascals_t = units::derived_t<Newtons_t, units::Length<-2>>;
using MPam05_t = units::derived_t<units::mega, Pascals_t, units::Length<1, 2>>;
