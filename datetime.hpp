#pragma once

#include <chrono>
#include <iostream>
#include <sstream>

using TimeBaseUnit = float;
/*
template<class Ratio>
class BaseTime : public std::chrono::duration<TimeBaseUnit, Ratio> {
    using std::chrono::duration<TimeBaseUnit, Ratio>::duration;
};*/
class MilliSeconds
    : public std::chrono::duration<TimeBaseUnit, std::ratio<1, 60>> {
  using std::chrono::duration<TimeBaseUnit, std::ratio<1, 60>>::duration;

public:
  static std::string symbol() { return "ms"; }
  static std::string name() { return "milliseconds"; }
};

class Seconds : public std::chrono::duration<TimeBaseUnit> {
  using std::chrono::duration<TimeBaseUnit>::duration;

public:
  static std::string symbol() { return "secs"; }
  static std::string name() { return "seconds"; }
};

class Minutes : public std::chrono::duration<TimeBaseUnit, std::ratio<60, 1>> {
  using std::chrono::duration<TimeBaseUnit, std::ratio<60, 1>>::duration;

public:
  static std::string symbol() { return "mins"; }
  static std::string name() { return "minutes"; }
};

class Hours : public std::chrono::duration<TimeBaseUnit, std::ratio<3600, 1>> {
  using std::chrono::duration<TimeBaseUnit, std::ratio<3600, 1>>::duration;

public:
  static std::string symbol() { return "hrs"; }
  static std::string name() { return "hours"; }
};

class Days
    : public std::chrono::duration<TimeBaseUnit, std::ratio<24 * 3600, 1>> {
  using std::chrono::duration<TimeBaseUnit, std::ratio<24 * 3600, 1>>::duration;

public:
  static std::string symbol() { return "days"; }
  static std::string name() { return "days"; }
};

inline std::ostream& operator<<(std::ostream& os, MilliSeconds ms) {
    os << ms.count() << " " << MilliSeconds::symbol();
    return os;
  }

inline std::ostream& operator<<(std::ostream& os, Seconds s) {
  os << s.count() << " " << Seconds::symbol();
  return os;
}

inline std::ostream& operator<<(std::ostream& os, Minutes s) {
  os << s.count() << " " << Minutes::symbol();
  return os;
}

inline std::ostream& operator<<(std::ostream& os, Hours s) {
  os << s.count() << " " << Hours::symbol();
  return os;
}

inline std::ostream& operator<<(std::ostream& os, Days s) {
  os << s.count() << " " << Days::symbol();
  return os;
}
/*
inline std::stringstream& operator<<(std::stringstream& os, Days s) {
  os << s.count() << " " << Days::name;
  return os;
}*/

template <class SourceDuration>
Seconds to_string_helper(Seconds s, std::stringstream& ss, bool force) {
  auto dur = SourceDuration{s};
  if (dur.count() > 0 || force) {
    const auto rounded = SourceDuration{int(dur.count())};
    ss << rounded << " ";
    s -= Seconds(rounded);
  }
  return s;
}

namespace std {
  std::string inline to_string(const Seconds& s) {
    std::stringstream ss;
    const auto days_removed = to_string_helper<Days>(s, ss, false);
    const auto hours_removed =
        to_string_helper<Hours>(days_removed, ss, days_removed != s);
    const auto mins_removed =
        to_string_helper<Minutes>(hours_removed, ss, hours_removed != s);
    const auto seconds_removed =
        to_string_helper<Seconds>(mins_removed, ss, mins_removed != s);
    to_string_helper<MilliSeconds>(seconds_removed, ss, seconds_removed != s);

    return ss.str();
  }
}