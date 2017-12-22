#pragma once
#include "base_dimensions.hpp"
#include <boost/hana.hpp>
#include <cassert>
#include <exception>
#include <type_traits>

namespace si {

  /** Compile time class which holds a std::ratio for each */
  template <class Length, class Mass, class Time, class Current,
            class Temperature, class Amount, class Luminosity>
  struct Dimensions {
    using length = Length;
    using mass = Mass;
    using time = Time;
    using current = Current;
    using temperature = Temperature;
    using amount = Amount;
    using luminosity = Luminosity;
    static_assert(is_length(length{}));
    static_assert(is_mass(mass{}));
    static_assert(is_time(time{}));
    static_assert(is_current(current{}));
    static_assert(is_temperature(temperature{}));
    static_assert(is_amount(amount{}));
    static_assert(is_luminosity(luminosity{}));
  };
  template <class Arg>
  constexpr std::false_type is_dimensions(Arg) {
    return std::false_type{};
  }

  template <class Arg0, class Arg1, class Arg2, class Arg3, class Arg4,
            class Arg5, class Arg6>
  constexpr std::true_type
  is_dimensions(Dimensions<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>) {
    return std::true_type{};
  }

  // Fwd declartion
  template <class... Args>
  struct derived;

  template <class Arg>
  constexpr std::false_type is_derived(Arg) {
    return std::false_type{};
  }

  template <class... Args>
  constexpr std::true_type is_derived(derived<Args...>) {
    return std::true_type{};
  }

  namespace Impl {

    template <template <class, class> class BinOp, class Le0, class M0,
              class Ti0, class C0, class Te0, class A0, class Lu0, class Le1,
              class M1, class Ti1, class C1, class Te1, class A1, class Lu1>
    constexpr auto
    dimensions_operator(Dimensions<Le0, M0, Ti0, C0, Te0, A0, Lu0>,
                        Dimensions<Le1, M1, Ti1, C1, Te1, A1, Lu1>) {
      return Dimensions<BinOp<Le0, Le1>, BinOp<M0, M1>, BinOp<Ti0, Ti1>,
                        BinOp<C0, C1>, BinOp<Te0, Te1>, BinOp<A0, A1>,
                        BinOp<Lu0, Lu1>>{};
    }

    template <class... Args0, class... Args1>
    constexpr auto multiply(Dimensions<Args0...> a = Dimensions<Args0...>{}, Dimensions<Args1...> b = Dimensions<Args1...>{}) {
      return dimensions_operator<std::ratio_add>(a, b);
    }

    template <class... Args0, class... Args1>
    constexpr auto divide(Dimensions<Args0...> a, Dimensions<Args1...> b) {
      return dimensions_operator<std::ratio_subtract>(a, b);
    }

    struct DimensionCounter {
      struct runtime_ratio {
        intmax_t n;
        intmax_t d;
        constexpr runtime_ratio() : n{0}, d{1} {}
        constexpr runtime_ratio(intmax_t a, intmax_t b) : n{a}, d{b} {}
        constexpr runtime_ratio& operator+=(const runtime_ratio& other) {
          n = n * other.d + other.n * d;
          d *= other.d;
          return *this;
        }
        template <intmax_t N, intmax_t D>
        constexpr runtime_ratio& operator+=(std::ratio<N, D>) {
          n = n * D + N * d;
          d *= D;
          return *this;
        }
        
        constexpr runtime_ratio& operator*=(const runtime_ratio& other) {
          n *= other.n;
          d *= other.d;
          return *this;
        }
        template <intmax_t N, intmax_t D>
        constexpr runtime_ratio& operator*=(std::ratio<N, D>) {
          n *= N;
          d *= D;
          return *this;
        }
      };
      struct runtime_base {
          runtime_ratio exp{0, 1};
          runtime_ratio prefix{1,1};
          constexpr runtime_base& operator+=(const runtime_base& other) {
              exp += other.exp;
              prefix *= other.prefix;
              return *this;
          }
      };
      
      runtime_base length{}, mass{}, time{}, current{}, temperature{}, amount{}, luminosity{};
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
    };

    template <class Arg>
    constexpr DimensionCounter parse_base_unit(Arg arg) {
      auto count = DimensionCounter{};
      if constexpr (is_length(arg)) {
        count.length.exp    += Arg::exp;
        count.length.prefix *= Arg::prefix;
      } else if constexpr (is_time(arg)) {
         count.time.exp    += Arg::exp;
        count.time.prefix *= Arg::prefix;
      } else if constexpr (is_mass(arg)) {
        count.mass.exp    += Arg::exp;
        count.mass.prefix *= Arg::prefix;
      } else if constexpr (is_temperature(arg)) {
        count.temperature.exp    += Arg::exp;
        count.temperature.prefix *= Arg::prefix;
      } else if constexpr (is_amount(arg)) {
        count.amount.exp    += Arg::exp;
        count.amount.prefix *= Arg::prefix;
      } else if constexpr (is_luminosity(arg)) {
        count.luminosity.exp    += Arg::exp;
        count.luminosity.prefix *= Arg::prefix;
      } else if constexpr (is_current(arg)) {
        count.current.exp    += Arg::exp;
        count.current.prefix *= Arg::prefix;
      } else {
        assert(false);
      }
      return count;
    } // namespace Impl

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
        // throw std::logic_error("Oops");
        // std::cout << "Oops\n";
        // assert(false);
      }
      return count;
    }

    /** At compile time create a dimension counter and add all of the Args to
     * it. */
    template <class... Args>
    constexpr auto parse_units() {
      auto count = DimensionCounter{};
      constexpr auto args = boost::hana::tuple<Args...>{};
      boost::hana::for_each(
          args, [&count](auto arg) { count += Impl::parse_arg(arg); });

      return count;
    }
  } // namespace Impl

  /** Collect a number of derived or base dimension classes into a single one.
   */
  template <class Arg0, class... Args>
  constexpr auto parse_units() {
    if constexpr (is_dimensions(Arg0{})) {
      return Arg0{};
    } else {
      constexpr auto count = Impl::parse_units<Arg0, Args...>();
      return Dimensions<std::ratio<count.length.n, count.length.d>,
                        std::ratio<count.mass.n, count.mass.d>,
                        std::ratio<count.time.n, count.time.d>,
                        std::ratio<count.current.n, count.current.d>,
                        std::ratio<count.temperature.n, count.temperature.d>,
                        std::ratio<count.amount.n, count.amount.d>,
                        std::ratio<count.luminosity.n, count.luminosity.d>>{};
    }
  }
  /** Collect a number of dereived or base dimension classes into a single one
   * using the parse_units constexpr function.
   * If passed a Dimensions class will just inherit from it. */
  template <class... Args>
  struct derived : decltype(parse_units<Args...>()) {
    using Base = decltype(parse_units<Args...>());
    constexpr static auto Units = boost::hana::tuple<Args...>{};
    static_assert(boost::hana::all_of(Units, [](auto arg) {
      return is_base_dimension(arg) || is_derived(arg) || is_dimensions(arg);
    }));
    // static_assert(is_base_dimension<Args...>);
    // : public decltype(parse_units<Args...>()) {
    // template <class... Args1>
    // constexpr auto multiply(derived<Args1...> other) {

    //}
  };
  template <class... Args0, class... Args1>
  constexpr auto operator*(derived<Args0...> a, derived<Args1...> b) {
    change code to only use derived type to construct things, everything else should use Dimensions directly?
    using new_dimensions = decltype(Impl::multiply<typename derived<Args0...>::Base, typename derived<Args1...>::Base>());;
    return derived<new_dimensions>{};
  }
  /*
  // template <template <class... Args0> class Der0,  template <class... Args1>
  // class Der1>
  template <class Derived0, class Derived1,
            std::enable_if_t<std::is_base_of_v<Derived0, Dimensions> &&
                             std::is_base_of_v<Derived1, Dimensions>>>
  constexpr bool operator*(Derived0 a, Derived1 b) {
    return false;
  }*/

  // Deduction guide
  template <class... Args>
  derived(Args... args)->derived<Args...>;

  static_assert(!is_derived(length<1>{}));
  static_assert(is_derived(derived<length<1>>{}));
  static_assert(is_derived(derived<length<1>, mass<1>>{}));

} // namespace si