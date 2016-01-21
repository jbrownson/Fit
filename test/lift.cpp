#include "test.hpp"
#include <boost/fit/lift.hpp>
#include <tuple>

template<class T, class U>
constexpr T sum(T x, U y)
{
    return x + y;
}

BOOST_FIT_LIFT_CLASS(max_f, std::max);
BOOST_FIT_LIFT_CLASS(sum_f, sum);

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(max_f()(3, 4) == std::max(3, 4));

    BOOST_FIT_TEST_CHECK(sum_f()(1, 2) == 3);
    BOOST_FIT_STATIC_TEST_CHECK(sum_f()(1, 2) == 3);
}

#if BOOST_FIT_HAS_GENERIC_LAMBDA
BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(BOOST_FIT_LIFT(std::max)(3, 4) == std::max(3, 4));
    BOOST_FIT_TEST_CHECK(BOOST_FIT_LIFT(sum)(1, 2) == 3);
}
#endif
