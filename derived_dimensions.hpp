#pragma once
#include "base_dimensions.hpp"
#include <boost/hana.hpp>
#include <cassert>
#include <exception>
#include <type_traits>

namespace si {
  template <class Length, class Mass, class Time, class Current,
            class Temperature, class Amount, class Luminosity>
  struct BaseDimensions {
    using length = Length;
    using mass = Mass;
    using time = Time;
    using current = Current;
    using temperature = Temperature;
    using amount = Amount;
    using luminosity = Luminosity;
  };

  /** Compile time class which holds a std::ratio for each */
  template <class Length, class Mass, class Time, class Current,
            class Temperature, class Amount, class Luminosity, class Prefix>
  struct Dimensions {
    using length = Length;
    using mass = Mass;
    using time = Time;
    using current = Current;
    using temperature = Temperature;
    using amount = Amount;
    using luminosity = Luminosity;
    using prefix = Prefix;
  };

  template <class BaseDim, class Prefix>
  struct BaseDimensionToDimension {
    using type =
        Dimensions<typename BaseDim::length, typename BaseDim::mass,
                   typename BaseDim::time, typename BaseDim::current,
                   typename BaseDim::temperature, typename BaseDim::amount,
                   typename BaseDim::luminosity, Prefix>;
  };
  template <class Dim>
  struct DimensionToBaseDimension {
    using type = BaseDimensions<typename Dim::length, typename Dim::mass,
                                typename Dim::time, typename Dim::current,
                                typename Dim::temperature, typename Dim::amount,
                                typename Dim::luminosity>;
  };

  template <class Le0, class M0, class Ti0, class C0, class Te0, class A0,
            class Lu0, class Le1, class M1, class Ti1, class C1, class Te1,
            class A1, class Lu1, class Pr0, class Pr1>
  constexpr auto
  same_dimension(Dimensions<Le0, M0, Ti0, C0, Te0, A0, Lu0, Pr0>,
                 Dimensions<Le1, M1, Ti1, C1, Te1, A1, Lu1, Pr1>) {
    if constexpr (std::ratio_equal_v<Le0, Le1> && std::ratio_equal_v<M0, M1> &&
                  std::ratio_equal_v<Ti0, Ti1> && std::ratio_equal_v<C0, C1> &&
                  std::ratio_equal_v<Te0, Te1> && std::ratio_equal_v<A0, A1> &&
                  std::ratio_equal_v<Lu0, Lu1>) {
      return std::true_type{};
    } else {
      return std::false_type{};
    }
  }

  template <class Arg>
  constexpr std::false_type is_dimensions(Arg) {
    return std::false_type{};
  }

  template <class Arg0, class Arg1, class Arg2, class Arg3, class Arg4,
            class Arg5, class Arg6, class Arg7>
  constexpr std::true_type
  is_dimensions(Dimensions<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>) {
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

    template <template <class, class> class BinOpDim,
              template <class, class> class BinOpPre, class Le0, class M0,
              class Ti0, class C0, class Te0, class A0, class Lu0, class Le1,
              class M1, class Ti1, class C1, class Te1, class A1, class Lu1,
              class Pr0, class Pr1>
    constexpr auto
    dimensions_operator(Dimensions<Le0, M0, Ti0, C0, Te0, A0, Lu0, Pr0>,
                        Dimensions<Le1, M1, Ti1, C1, Te1, A1, Lu1, Pr1>) {
      return Dimensions<BinOpDim<Le0, Le1>, BinOpDim<M0, M1>,
                        BinOpDim<Ti0, Ti1>, BinOpDim<C0, C1>,
                        BinOpDim<Te0, Te1>, BinOpDim<A0, A1>,
                        BinOpDim<Lu0, Lu1>, BinOpPre<Pr0, Pr1>>{};
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
        runtime_ratio prefix{1, 1};
        constexpr runtime_base& operator+=(const runtime_base& other) {
          exp += other.exp;
          prefix *= other.prefix;
          return *this;
        }
      };

      runtime_base length{}, mass{}, time{}, current{}, temperature{}, amount{},
          luminosity{};
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
      constexpr DimensionCounter() = default;

      template <class Le, class M, class Ti, class C, class Te, class A,
                class Lu, class Pr>
      constexpr DimensionCounter(const Dimensions<Le, M, Ti, C, Te, A, Lu, Pr>&)
          : length{{Le::num, Le::den}, {Pr::num, Pr::den}},
            mass{{M::num, M::den}, {1, 1}},
            time{{Ti::num, Ti::den}, {1, 1}},
            current{{C::num, C::den}, {1, 1}},
            temperature{{Te::num, Te::den}, {1, 1}},
            amount{{A::num, A::den}, {1, 1}},
            luminosity{{Lu::num, Lu::den}, {1, 1}} {}

      template <intmax_t N, intmax_t D>
      constexpr auto add_prefix(std::ratio<N, D> r) {
        luminosity.prefix *= r;
      }
    };

    template <class Arg>
    constexpr DimensionCounter parse_base_unit(Arg arg) {
      auto count = DimensionCounter{};
      if constexpr (is_length(arg)) {
        count.length.exp += Arg::exp;
        count.length.prefix *= Arg::prefix;
      } else if constexpr (is_time(arg)) {
        count.time.exp += Arg::exp;
        count.time.prefix *= Arg::prefix;
      } else if constexpr (is_mass(arg)) {
        count.mass.exp += Arg::exp;
        count.mass.prefix *= Arg::prefix;
      } else if constexpr (is_temperature(arg)) {
        count.temperature.exp += Arg::exp;
        count.temperature.prefix *= Arg::prefix;
      } else if constexpr (is_amount(arg)) {
        count.amount.exp += Arg::exp;
        count.amount.prefix *= Arg::prefix;
      } else if constexpr (is_luminosity(arg)) {
        count.luminosity.exp += Arg::exp;
        count.luminosity.prefix *= Arg::prefix;
      } else if constexpr (is_current(arg)) {
        count.current.exp += Arg::exp;
        count.current.prefix *= Arg::prefix;
      } else {
        assert(false);
      }
      return count;
    } // namespace Impl

    template <class Arg>
    constexpr DimensionCounter parse_arg(Arg arg) {
      auto count = DimensionCounter{};
      if constexpr (si::is_base_dimension(arg)) {
        count += parse_base_unit(arg);
      } else if constexpr (si::is_dimensions(arg)) {
        constexpr auto rt = DimensionCounter(arg);
        count += rt;
        // throw std::logic_error("Oops");
        // std::cout << "Oops\n";
        // assert(false);
      } else if constexpr (is_ratio(arg)) {
        count.add_prefix(arg);
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
    constexpr auto combine_prefixes(DimensionCounter count) {
      auto prefix = count.length.prefix;
      prefix *= count.mass.prefix;
      prefix *= count.time.prefix;
      prefix *= count.current.prefix;
      prefix *= count.temperature.prefix;
      prefix *= count.amount.prefix;
      prefix *= count.luminosity.prefix;
      return prefix;
    }
  } // namespace Impl
  template <class... Args0, class... Args1>
  constexpr auto
  operator*(Dimensions<Args0...> a,   // = Dimensions<Args0...>{},
            Dimensions<Args1...> b) { //} = Dimensions<Args1...>{}) {
    return Impl::dimensions_operator<std::ratio_add, std::ratio_multiply>(a, b);
  }

  template <class... Args0, class... Args1>
  constexpr auto operator/(Dimensions<Args0...> a, Dimensions<Args1...> b) {
    return Impl::dimensions_operator<std::ratio_subtract, std::ratio_divide>(a,
                                                                             b);
  }

  /** Collect a number of derived or base dimension classes into a single one.
   */
  template <class Arg0, class... Args>
  constexpr auto parse_units() {
    constexpr auto count = Impl::parse_units<Arg0, Args...>();
    constexpr auto prefix = Impl::combine_prefixes(count);
    return Dimensions<
        std::ratio<count.length.exp.n, count.length.exp.d>,
        std::ratio<count.mass.exp.n, count.mass.exp.d>,
        std::ratio<count.time.exp.n, count.time.exp.d>,
        std::ratio<count.current.exp.n, count.current.exp.d>,
        std::ratio<count.temperature.exp.n, count.temperature.exp.d>,
        std::ratio<count.amount.exp.n, count.amount.exp.d>,
        std::ratio<count.luminosity.exp.n, count.luminosity.exp.d>,
        std::ratio<prefix.n, prefix.d>>{};
  }

  template <class Arg0, class... Args>
  constexpr auto parse_units_unity_prefix() {
    constexpr auto count = Impl::parse_units<Arg0, Args...>();
    return Dimensions<
        std::ratio<count.length.exp.n, count.length.exp.d>,
        std::ratio<count.mass.exp.n, count.mass.exp.d>,
        std::ratio<count.time.exp.n, count.time.exp.d>,
        std::ratio<count.current.exp.n, count.current.exp.d>,
        std::ratio<count.temperature.exp.n, count.temperature.exp.d>,
        std::ratio<count.amount.exp.n, count.amount.exp.d>,
        std::ratio<count.luminosity.exp.n, count.luminosity.exp.d>,
        std::ratio<1, 1>>{};
  }
  /** Convert base dimensions or derived types into a single Dimension class in
   * its "type" alias.*/
  template <class... Args>
  struct derived {
    using type = decltype(parse_units<Args...>());
    constexpr static auto Units = boost::hana::tuple<Args...>{};
    static_assert(boost::hana::all_of(Units, [](auto arg) {
      return is_base_dimension(arg) || is_derived(arg) || is_dimensions(arg) ||
             is_ratio(arg);
    }));
  };

  template <class... Args>
  struct derived_unity {
    using type = decltype(parse_units_unity_prefix<Args...>());
    constexpr static auto Units = boost::hana::tuple<Args...>{};
    static_assert(boost::hana::all_of(Units, [](auto arg) {
      return is_base_dimension(arg) || is_derived(arg) || is_dimensions(arg) ||
             is_ratio(arg);
    }));
  };

  template <class... Args>
  using derived_t = typename derived<Args...>::type;

  template <class... Args>
  using derived_unity_t = typename derived_unity<Args...>::type;

  static_assert(!is_derived(Length<1>{}));
  static_assert(is_derived(derived<Length<1>>{}));
  static_assert(is_derived(derived<Length<1>, Mass<1>>{}));

} // namespace si