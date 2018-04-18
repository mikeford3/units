#pragma once
#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "prefixes.hpp"

using m_t = units::derived_t<units::Length<1, 1, units::unity>>;
using cm_t = units::derived_t<
    units::Length<1, 1, std::ratio_multiply<m_t::prefix, units::centi>>>;
using km_t = units::derived_t<
    units::Length<1, 1, std::ratio_multiply<m_t::prefix, units::kilo>>>;
using seconds_t = units::derived_t<units::Time<1, 1, units::unity>>;
using kg_t = units::derived_t<units::Mass<1, 1, units::unity>>;
using velocity_t1 = units::derived_t<units::Length<1>, units::Time<-1>>;
using velocity_t2 = units::derived_t<m_t, units::Time<-1>>;
using acceleration_t = units::derived_t<velocity_t2, units::Time<-1>>;
using N_t = units::derived_t<kg_t, acceleration_t>;
using Pa_t = units::derived_t<N_t, units::Length<-2>>;
using MPam05_t = units::derived_t<units::mega, Pa_t, units::Length<1, 2>>;
using m2_t = units::derived_t<units::Length<2>>;
using m3_t = units::derived_t<units::Length<3>>;
using m05_t = units::derived_t<units::Length<1, 2>>;
