#pragma once
#include "base_dimensions.hpp"
#include <boost/hana.hpp>
#include <cassert>
#include <exception>
#include <type_traits>

namespace units {
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

  template <class Arg0, class Arg1, class Arg2, class Arg3, class Arg4,
            class Arg5, class Arg6, class Prefix>
  constexpr auto is_dimensionless(
      Dimensions<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Prefix>) {
    if constexpr (Arg0::num == 0 && Arg1::num == 0 && Arg2::num == 0 &&
                  Arg3::num == 0 && Arg4::num == 0 && Arg5::num == 0 &&
                  Arg6::num == 0) {
      return std::true_type{};
    } else {
      return std::false_type{};
    }
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
        intmax_t n = 0;
        intmax_t d = 1;
        constexpr runtime_ratio() = default;
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

      runtime_ratio length{}, mass{}, time{}, current{}, temperature{},
          amount{}, luminosity{}, prefix{1, 1};
      constexpr DimensionCounter& operator+=(const DimensionCounter& other) {
        length += other.length;
        mass += other.mass;
        time += other.time;
        current += other.current;
        temperature += other.temperature;
        amount += other.amount;
        luminosity += other.luminosity;
        prefix *= other.prefix;
        return *this;
      }
      constexpr DimensionCounter() = default;

      template <class Le, class M, class Ti, class C, class Te, class A,
                class Lu, class Pr>
      constexpr DimensionCounter(const Dimensions<Le, M, Ti, C, Te, A, Lu, Pr>&)
          : length{Le::num, Le::den},
            mass{M::num, M::den},
            time{Ti::num, Ti::den},
            current{C::num, C::den},
            temperature{Te::num, Te::den},
            amount{A::num, A::den},
            luminosity{Lu::num, Lu::den},
            prefix{Pr::num, Pr::den} {}

      template <intmax_t N, intmax_t D>
      constexpr auto add_prefix(std::ratio<N, D> r) {
        prefix *= r;
      }
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
      } else if constexpr (is_temperature(arg)) {
        count.temperature += Arg::exp;
      } else if constexpr (is_amount(arg)) {
        count.amount += Arg::exp;
      } else if constexpr (is_luminosity(arg)) {
        count.luminosity += Arg::exp;
      } else if constexpr (is_current(arg)) {
        count.current += Arg::exp;
      } else {
        assert(false);
      }
      return count;
    } // namespace Impl

    template <class Arg>
    constexpr DimensionCounter parse_arg(Arg arg) {
      auto count = DimensionCounter{};
      if constexpr (units::is_base_dimension(arg)) {
        count += parse_base_unit(arg);
      } else if constexpr (units::is_dimensions(arg)) {
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

  } // namespace Impl

  template <class... Args0, class... Args1>
  constexpr auto operator*(Dimensions<Args0...> a, Dimensions<Args1...> b) {
    return Impl::dimensions_operator<std::ratio_add, std::ratio_multiply>(a, b);
  }

  template <class... Args0, class... Args1>
  constexpr auto operator/(Dimensions<Args0...> a, Dimensions<Args1...> b) {
    return Impl::dimensions_operator<std::ratio_subtract, std::ratio_divide>(a,
                                                                             b);
  }

  template <class Le, class M, class Ti, class C, class Te, class A, class Lu,
            class Pr>
  constexpr auto sqrt([
      [maybe_unused]] const units::Dimensions<Le, M, Ti, C, Te, A, Lu, Pr>& a) {
    using two = std::ratio<2, 1>;
    // static_assert(std::is_same_v<units::unity, Pr>);
    using l = std::ratio_divide<Le, two>;
    using m = std::ratio_divide<M, two>;
    using t = std::ratio_divide<Ti, two>;
    using c = std::ratio_divide<C, two>;
    using te = std::ratio_divide<Te, two>;
    using am = std::ratio_divide<A, two>;
    using lu = std::ratio_divide<Lu, two>;
    return units::Dimensions<l, m, t, c, te, am, lu, Pr>{};
  }

  template <class Le, class M, class Ti, class C, class Te, class A, class Lu,
            class Pr>
  constexpr auto invert([
      [maybe_unused]] const units::Dimensions<Le, M, Ti, C, Te, A, Lu, Pr>& a) {
    using minus_1 = std::ratio<-1, 1>;
    // static_assert(std::is_same_v<units::unity, Pr>);
    using l = std::ratio_multiply<Le, minus_1>;
    using m = std::ratio_multiply<M, minus_1>;
    using t = std::ratio_multiply<Ti, minus_1>;
    using c = std::ratio_multiply<C, minus_1>;
    using te = std::ratio_multiply<Te, minus_1>;
    using am = std::ratio_multiply<A, minus_1>;
    using lu = std::ratio_multiply<Lu, minus_1>;
    using pr = std::ratio_divide<std::ratio<1, 1>, Pr>;
    return units::Dimensions<l, m, t, c, te, am, lu, pr>{};
  }

  /** Collect a number of derived or base dimension classes into a single one.
   */
  template <class Arg0, class... Args>
  constexpr auto parse_units() {
    constexpr auto count = Impl::parse_units<Arg0, Args...>();
    return Dimensions<
        // units::Length<count.length.exp.n, count.length.exp.d>,
        std::ratio<count.length.n, count.length.d>,
        std::ratio<count.mass.n, count.mass.d>,
        std::ratio<count.time.n, count.time.d>,
        std::ratio<count.current.n, count.current.d>,
        std::ratio<count.temperature.n, count.temperature.d>,
        std::ratio<count.amount.n, count.amount.d>,
        std::ratio<count.luminosity.n, count.luminosity.d>,
        std::ratio<count.prefix.n, count.prefix.d>>{};
  }
  /*constexpr auto parse_units() {
    return Dimensions{};
  }*/

  template <class Arg0, class... Args>
  constexpr auto parse_units_unity_prefix() {
    constexpr auto count = Impl::parse_units<Arg0, Args...>();
    return Dimensions<std::ratio<count.length.n, count.length.d>,
                      std::ratio<count.mass.n, count.mass.d>,
                      std::ratio<count.time.n, count.time.d>,
                      std::ratio<count.current.n, count.current.d>,
                      std::ratio<count.temperature.n, count.temperature.d>,
                      std::ratio<count.amount.n, count.amount.d>,
                      std::ratio<count.luminosity.n, count.luminosity.d>,
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
    /*template<class Other>
    static constexpr auto per(Other other = Other{}) {
      using Inverted = invert(std::move(other));


    }*/
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

} // namespace units