#pragma once
#include "base_dimensions.hpp"
#include "derived_dimensions.hpp"
#include "prefixes.hpp"

using m_t = si::derived_t<si::Length<1, 1, si::unity>>;
using cm_t = si::derived_t<
    si::Length<1, 1, std::ratio_multiply<m_t::prefix, si::centi>>>;
using km_t =
    si::derived_t<si::Length<1, 1, std::ratio_multiply<m_t::prefix, si::kilo>>>;
using seconds_t = si::derived_t<si::Time<1, 1, si::unity>>;
using kg_t = si::derived_t<si::Mass<1, 1, si::unity>>;
using velocity_t1 = si::derived_t<si::Length<1>, si::Time<-1>>;
using velocity_t2 = si::derived_t<m_t, si::Time<-1>>;
using acceleration_t = si::derived_t<velocity_t2, si::Time<-1>>;
using N_t = si::derived_t<kg_t, acceleration_t>;
using Pa_t = si::derived_t<N_t, si::Length<-2>>;
using MPam05_t = si::derived_t<Pa_t, si::Length<1, 2>>;
