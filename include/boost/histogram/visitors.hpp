#ifndef _BOOST_HISTOGARM_VISITORS_HPP_
#define _BOOST_HISTOGARM_VISITORS_HPP_

#include <boost/histogram/axis.hpp>
#include <boost/variant/static_visitor.hpp>

namespace boost {
namespace histogram {
namespace visitor {

  struct bins : public static_visitor<unsigned>
  {
    template <typename A>
    unsigned operator()(const A& a) const { return a.bins(); }
  };

  struct shape : public static_visitor<unsigned>
  {
    template <typename A>
    unsigned operator()(const A& a) const { return a.bins() + 2 * a.uoflow(); }
  };

  struct uoflow : public static_visitor<bool>
  {
    template <typename A>
    bool operator()(const A& a) const { return a.uoflow(); }
  };

  struct index : public static_visitor<int>
  {
    double x;
    index(double d) : x(d) {}

    template <typename A>
    int operator()(const A& a) const { return a.index(x); }

    int operator()(const category_axis& a) const { return static_cast<int>(x + 0.5); }
  };

  struct cmp : public static_visitor<bool>
  {
    template <typename T>
    bool operator()(const T& a, const T& b) const
    { return a == b; }

    template <typename T, typename U>
    bool operator()(const T&, const U&) const { return false; }
  };
}
}
}

#endif
