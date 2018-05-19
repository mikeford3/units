#pragma once
#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "prefixes.hpp"

using m_t = units::derived_t<units::Length<1, 1, units::unity>>;
using cm_t = units::derived_t<
    units::Length<1, 1, std::ratio_multiply<m_t::prefix, units::centi>>>;
using km_t = units::derived_t<
    units::Length<1, 1, std::ratio_multiply<m_t::prefix, units::kilo>>>;

using m2_t = units::derived_t<units::Length<2>>;
using acre_t = units::derived_t<
    m2_t, std::ratio_multiply<m_t::prefix, std::ratio<404'685'642, 1>>>;
using hectare_t =
    units::derived_t<m2_t,
                     std::ratio_multiply<m_t::prefix, std::ratio<10'000, 1>>>;

using seconds_t = units::derived_t<units::Time<1, 1, units::unity>>;
using per_second_t = units::derived_t<units::Time<-1, 1, units::unity>>;
using minutes_t = units::derived_t<units::Time<
    1, 1, std::ratio_multiply<seconds_t::prefix, std::ratio<60, 1>>>>;
using per_min_t = units::derived_t<units::Time<
    1, 1, std::ratio_multiply<seconds_t::prefix, std::ratio<60, 1>>>>;
using hours_t = units::derived_t<units::Time<
    1, 1, std::ratio_multiply<seconds_t::prefix, std::ratio<3600, 1>>>>;
using per_hour_t = units::derived_t<units::Time<
    1, 1, std::ratio_multiply<seconds_t::prefix, std::ratio<3600, 1>>>>;
using days_t = units::derived_t<
    units::Time<1, 1, std::ratio_multiply<hours_t::prefix, std::ratio<24, 1>>>>;
using per_days_t = units::derived_t<
    units::Time<1, 1, std::ratio_multiply<hours_t::prefix, std::ratio<24, 1>>>>;

using kg_t = units::derived_t<units::Mass<1, 1, units::unity>>;
using velocity_t1 = units::derived_t<units::Length<1>, units::Time<-1>>;
using velocity_t2 = units::derived_t<m_t, units::Time<-1>>;
using acceleration_t = units::derived_t<velocity_t2, units::Time<-1>>;
using N_t = units::derived_t<kg_t, acceleration_t>;
using Pa_t = units::derived_t<N_t, units::Length<-2>>;
using MPam05_t = units::derived_t<units::mega, Pa_t, units::Length<1, 2>>;

using m3_t = units::derived_t<units::Length<3>>;
using m05_t = units::derived_t<units::Length<1, 2>>;
using litre_t = units::derived_t<m3_t, units::milli>;
using us_gallon_t =
    units::derived_t<litre_t, std::ratio<37'854'118, 10'000'000>>;
using imp_gallon_t = units::derived_t<litre_t, std::ratio<454'609, 100'000>>;
