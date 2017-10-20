#pragma once
#include <boost/hana.hpp>
#include <cassert>
#include <iostream>

#include "base_dimensions.hpp"

namespace si {
  template <int Length, int Mass, int Time, int Current, int Temperature,
            int Amount, int Luminosity>
  struct Dimensions {
    static constexpr int length = Length;
    static constexpr int mass = Mass;
    static constexpr int time = Time;
    static constexpr int current = Current;
    static constexpr int temperature = Temperature;
    static constexpr int amount = Amount;
    static constexpr int luminosity = Luminosity;
  };

  template <int L, int M, int T, int C, int Te, int A, int Lu>
  std::ostream& operator<<(std::ostream& os,
                           Dimensions<L, M, T, C, Te, A, Lu>) {
    os << "m^" << Dimensions<L, M, T, C, Te, A, Lu>::length      //make
       << "kg^" << Dimensions<L, M, T, C, Te, A, Lu>::mass       //power
       << "s^" << Dimensions<L, M, T, C, Te, A, Lu>::time        //follow
       << "A^" << Dimensions<L, M, T, C, Te, A, Lu>::current     //unit
       << "K^" << Dimensions<L, M, T, C, Te, A, Lu>::temperature //
       << "mol^" << Dimensions<L, M, T, C, Te, A, Lu>::amount    //
       << "cd^" << Dimensions<L, M, T, C, Te, A, Lu>::luminosity;
    return os;
  }

  template <class... Args>
  struct derived;

  template <class Arg>
  constexpr std::false_type is_derived(Arg);

  template <class... Args>
  constexpr std::true_type is_derived(derived<Args...>);

  namespace Impl {
    struct DimensionCounter {
      int length = 0;
      int mass = 0;
      int time = 0;
      int current = 0;
      int temperature = 0;
      int amount = 0;
      int luminosity = 0;
      constexpr DimensionCounter& operator+=(const DimensionCounter& other) {
        length += other.length;
        mass += other.mass;
        time += other.time;
        current += other.current;
        temperature += other.temperature;
        amount += other.amount;
        luminosity += other.luminosity;
        return *this;
      }
      /*template <int N>
      void add(length<N>) {
        length += N;
      }
      void add(time<T>) { time += N;}*/
    };

    template <class Arg>
    constexpr DimensionCounter parse_base_unit(Arg arg) {
      auto count = DimensionCounter{};
      if constexpr (is_length(arg)) {
        count.length += Arg::exp;
      } else if constexpr (is_time(arg)) {
        count.time += Arg::exp;
      } else if constexpr (is_mass(arg)) {
        count.mass += Arg::exp;
      } else {
        assert(false);
      }
      return count;
    }

    template <class Arg>
    constexpr DimensionCounter parse_arg(Arg arg) {
      auto count = DimensionCounter{};
      if constexpr (si::is_derived(arg)) {
        boost::hana::for_each(Arg::Units, [&count](auto subarg) {
          count += Impl::parse_arg(subarg);
        });
      } else if constexpr (si::is_base_dimension(arg)) {
        count += parse_base_unit(arg);
      } else {
        assert(false);
      }
      return count;
    }

    template <class... Args>
    constexpr auto parse_units(Args... args) {
      auto count = DimensionCounter{};
      constexpr auto tup_args = boost::hana::make_tuple(args...);
      boost::hana::for_each(
          tup_args, [&count](auto arg) { count += Impl::parse_arg(arg); });
      return count;
    }

    template <class... Args>
    constexpr auto parse_units() {
      auto count = DimensionCounter{};
      constexpr auto args = boost::hana::tuple<Args...>{};
      boost::hana::for_each(
          args, [&count](auto arg) { count += Impl::parse_arg(arg); });

      return count;
    }
  } // namespace Impl

  template <class... Args>
  constexpr auto parse_units(Args... args) {
    constexpr auto count = Impl::parse_units(args...);
    return Dimensions<count.length, count.mass, count.time, count.current,
                      count.temperature, count.amount, count.luminosity>{};
  }

  template <class... Args>
  constexpr auto parse_units() {
    constexpr auto count = Impl::parse_units<Args...>();
    return Dimensions<count.length, count.mass, count.time, count.current,
                      count.temperature, count.amount, count.luminosity>{};
  }

  template <class... Args>
  struct derived : decltype(parse_units<Args...>()) {

    constexpr static auto Units = boost::hana::tuple<Args...>{};
    static_assert(boost::hana::all_of(Units, [](auto arg) {
      return is_base_dimension(arg);
    }));
    // static_assert(is_base_dimension<Args...>);
    // : public decltype(parse_units<Args...>()) {
  };

  template <class... Args>
  derived(Args... args)->derived<Args...>;

  template <class Arg>
  constexpr std::false_type is_derived(Arg) {
    return std::false_type{};
  }

  template <class... Args>
  constexpr std::true_type is_derived(derived<Args...>) {
    return std::true_type{};
  }

  static_assert(!is_derived(length<1>{}));
  static_assert(is_derived(derived<length<1>>{}));
  static_assert(is_derived(derived<length<1>, mass<1>>{}));
} // namespace si