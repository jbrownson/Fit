#include <boost/fit/infix.hpp>
#include "test.hpp"

struct sum_f
{
    template<class T, class U>
    constexpr T operator()(T x, U y) const
    {
        return x+y;
    }
};

static constexpr boost::fit::infix_adaptor<sum_f> sum = {};

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(3 == (1 <sum> 2));
    BOOST_FIT_STATIC_TEST_CHECK(3 == (1 <sum> 2));
}
