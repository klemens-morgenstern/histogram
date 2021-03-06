#ifndef _BOOST_HISTOGRAM_HISTOGRAM_HPP_
#define _BOOST_HISTOGRAM_HISTOGRAM_HPP_

#include <boost/histogram/axis.hpp>
#include <boost/histogram/basic_histogram.hpp>
#include <boost/histogram/detail/nstore.hpp>
#include <boost/preprocessor.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>
#include <boost/move/move.hpp>
#include <stdexcept>

namespace boost {
namespace histogram {

class histogram : public basic_histogram {
  BOOST_COPYABLE_AND_MOVABLE(histogram)
public:
  histogram() {}

  explicit histogram(const axes_type& axes);

#define BOOST_HISTOGRAM_CTOR(z, n, unused)                                 \
  explicit histogram( BOOST_PP_ENUM_PARAMS_Z(z, n, const axis_type& a) ) : \
    basic_histogram( BOOST_PP_ENUM_PARAMS_Z(z, n, a) ),                    \
    data_(field_count())                                                   \
  {}

// generates constructors taking 1 to AXIS_LIMIT arguments
BOOST_PP_REPEAT_FROM_TO(1, BOOST_HISTOGRAM_AXIS_LIMIT, BOOST_HISTOGRAM_CTOR, nil)

  // copy semantics (implementation needs to be here, workaround for gcc-bug)
  histogram(const histogram& o) :
    basic_histogram(o),
    data_(o.data_)
  {}

  histogram& operator=(BOOST_COPY_ASSIGN_REF(histogram) o)
  {
    if (this != &o) {
      basic_histogram::operator=(static_cast<const basic_histogram&>(o));
      data_ = o.data_;
    }
    return *this;
  }

  // move semantics (implementation needs to be here, workaround for gcc-bug)
  histogram(BOOST_RV_REF(histogram) o) :
    basic_histogram(::boost::move(static_cast<basic_histogram&>(o))),
    data_(::boost::move(o.data_))
  {}

  histogram& operator=(BOOST_RV_REF(histogram) o)
  {
    if (this != &o) {
      basic_histogram::operator=(::boost::move(static_cast<basic_histogram&>(o)));
      data_ = ::boost::move(o.data_);
    }
    return *this;
  }

  // C-style call
  inline
  void fill_c(unsigned n, const double* v)
  {
    BOOST_ASSERT_MSG(n == dim(), "wrong number of arguments");
    const size_type k = pos(v);
    if (k != uintmax_t(-1))
      data_.increase(k);
  }

#define BOOST_HISTOGRAM_FILL(z, n, unused)                   \
  inline                                                     \
  void fill( BOOST_PP_ENUM_PARAMS_Z(z, n, double x) )        \
  {                                                          \
    const double buffer[n] = { BOOST_PP_ENUM_PARAMS(n, x) }; \
    fill_c(n, buffer); /* size is checked here */              \
  }

// generates fill functions taking 1 to AXIS_LIMT arguments
BOOST_PP_REPEAT_FROM_TO(1, BOOST_HISTOGRAM_AXIS_LIMIT, BOOST_HISTOGRAM_FILL, nil)  

  // C-style call
  inline
  void wfill_c(unsigned n, const double* v, double w)
  {
    BOOST_ASSERT_MSG(n == dim(), "wrong number of arguments");
    const size_type k = pos(v);
    if (k != uintmax_t(-1))
      data_.increase(k, w);
  }

#define BOOST_HISTOGRAM_WFILL(z, n, unused)                      \
  inline                                                         \
  void wfill( BOOST_PP_ENUM_PARAMS_Z(z, n, double x), double w ) \
  {                                                              \
    const double buffer[n] = { BOOST_PP_ENUM_PARAMS(n, x) };     \
    wfill_c(n, buffer, w); /* size is checked here */              \
  }

// generates wfill functions taking 1 to AXIS_LIMT arguments
BOOST_PP_REPEAT_FROM_TO(1, BOOST_HISTOGRAM_AXIS_LIMIT, BOOST_HISTOGRAM_WFILL, nil)  

  // C-style call
  inline
  double value_c(unsigned n, const int* idx)
    const
  {
    BOOST_ASSERT_MSG(n == dim(), "wrong number of arguments");
    return data_.value(linearize(idx));
  }

#define BOOST_HISTOGRAM_VALUE(z, n, unused)                 \
  inline                                                    \
  double value( BOOST_PP_ENUM_PARAMS_Z(z, n, int i) )       \
    const                                                   \
  {                                                         \
    const int idx[n] = { BOOST_PP_ENUM_PARAMS_Z(z, n, i) }; \
    return value_c(n, idx); /* size is checked here */        \
  }

// generates value functions taking 1 to AXIS_LIMT arguments
BOOST_PP_REPEAT_FROM_TO(1, BOOST_HISTOGRAM_AXIS_LIMIT, BOOST_HISTOGRAM_VALUE, nil)  

  // C-style call
  inline
  double variance_c(unsigned n, const int* idx)
    const
  {
    BOOST_ASSERT_MSG(n == dim(), "wrong number of arguments");
    return data_.variance(linearize(idx));
  }

#define BOOST_HISTOGRAM_VARIANCE(z, n, unused)              \
  inline                                                    \
  double variance( BOOST_PP_ENUM_PARAMS_Z(z, n, int i) )    \
    const                                                   \
  {                                                         \
    const int idx[n] = { BOOST_PP_ENUM_PARAMS_Z(z, n, i) }; \
    return variance_c(n, idx); /* size is checked here */     \
  }

// generates variance functions taking 1 to AXIS_LIMT arguments
BOOST_PP_REPEAT_FROM_TO(1, BOOST_HISTOGRAM_AXIS_LIMIT, BOOST_HISTOGRAM_VARIANCE, nil)  

  unsigned depth() const { return data_.depth(); }

  double sum() const;

  bool operator==(const histogram& o) const 
  { return basic_histogram::operator==(o) &&
           data_ == o.data_; }

  histogram& operator+=(const histogram& o)
  {
    if (!basic_histogram::operator==(o))
      throw std::logic_error("histograms have different axes");
    data_ += o.data_;
    return *this;
  }

private:
  detail::nstore data_;

  friend class serialization::access;
  template <class Archive>
  void serialize(Archive& ar, unsigned version)
  {
    ar & serialization::base_object<basic_histogram>(*this);
    ar & data_;
  }

  friend class histogram_access;
};

histogram operator+(const histogram& a, const histogram& b) {
  histogram result(a);
  return result += b;
}

}
}

#endif
