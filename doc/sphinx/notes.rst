Notes
=====

Dependencies
------------

* `Boost <http://www.boost.org>`_
* `CMake <https://cmake.org>`_

* **Optional dependencies**

  * `Python <http://www.python.org>`_ for Python bindings
  * `Numpy <http://www.numpy.org>`_ for Numpy support
  * `Sphinx <http://www.sphinx-doc.org>`_ to (re)build this documentation

How to build and install
------------------------
::

    git clone https://github.com/HDembinski/histogram.git
    mkdir build && cd build
    cmake ../histogram/build
    make install

Do ``make test`` to run the tests, or ``ctest -V`` for more output.

**Caveat**: I couldn't figure out a proper way to install the Python module with CMake, so for the time being, CMake will print a message with manual instructions instead. The main problem is how to pick the right dist-packages path in a platform-independent way, and such that it respects the ``CMAKE_INSTALL_PREFIX``.

Tests
-----

Most of the C++ interface is implicitly tested in the tests of the Python interface, which in turn calls the C++ interface.

Checks
------

Some checks are included in ``test/check``. These are not strictly tests, and not strictly examples, yet they provide useful information that belongs with the library code. They are not build by default, building can be activated with the CMake flag ``BUILD_CHECKS``.

Consistency of C++ and Python interface
---------------------------------------

The Python and C++ interface are indentical - except when they are not. The exceptions concern cases where a more elegant and pythonic way of implementing things exists. In a few cases, the C++ classes have extra member functions for convenience, which are not needed on the Python side.

Properties
    Getter/setter-like functions are wrapped as properties.

Keyword-based parameters
    C++ member functions :cpp:func:`histogram::fill` and :cpp:func:`histogram::wfill` are wrapped by the single Python member function :py:func:`histogram.fill` with an optional keyword parameter :py:obj:`w` to pass a weight.

C++ convenience
    C++ member function :cpp:func:`histogram::bins` is omitted on the Python side, since it is very easy to just query this directly from the axis object in Python. On the C++ side, this would require a extra type cast or applying a visitor.

Benchmarks
----------

One design goal of this project is to be fast. The act of filling the histogram with a number should be insignificant compared to the CPU cycles spend to retrieve/generate that number. Naturally, we also want to beat the competition.

The following table shows results of a simple benchmark against

* :cpp:type:`TH1I`, :cpp:type:`TH3I` and :cpp:type:`THnI` of the `ROOT framework <https://root.cern.ch>`_

* :py:func:`histogram` and :py:func:`histogramdd` from the Python module :py:mod:`numpy`

The benchmark against ROOT is implemented in C++, the benchmark against numpy in Python.

Remarks:

* The comparison with ROOT puts ROOT at the advantage, since :cpp:type:`TH1I` and :cpp:type:`TH3I` are specialized classes for 1 dimension and 3 dimensions, not a general class for N-dimensions like :cpp:class:`boost::histogram`. ROOT histograms also lack a comparably flexible system to define different binning schemes for each axis.

* Large vectors are pre-allocated and with random numbers drawn from a uniform or normal distribution for all tests. In the timed part, these numbers are read from the vector and put into the histograms. This reduces the overhead merely to memory access.

* The test with uniform random numbers never fills the overflow and underflow bins, while the test with random numbers from a normal distribution does. This explains some of the differences between the two distributions.

* All tests are repeated 10 times, the minimum is shown.

Test system: Intel Core i7-4500U CPU clocked at 1.8 GHz, 8 GB of DDR3 RAM

=================  =======  =======  =======  =======  =======  =======
distribution                uniform                    normal
-----------------  -------------------------  -------------------------
dimension          1D       3D       6D       1D       3D       6D
=================  =======  =======  =======  =======  =======  =======
No. of fills       12M      4M       2M       12M      4M       2M
C++: ROOT  [t/s]   0.127    0.199    0.185    0.168    0.143    0.179
C++: boost [t/s]   0.172    0.177    0.155    0.172    0.171    0.150
Py: numpy [t/s]    0.825    0.727    0.436    0.824    0.426    0.401
Py: boost [t/s]    0.209    0.229    0.192    0.207    0.194    0.168
=================  =======  =======  =======  =======  =======  =======

:cpp:class:`boost::histogram` shows consistent performance comparable to the specialized ROOT histograms. It is faster than ROOT's implementation of a N-dimensional histogram :cpp:type:`THnI`. The performance of :cpp:class:`boost::histogram` is similar in C++ and Python, showing only a small overhead in Python. It is consistently faster than numpy's histogram functions.
