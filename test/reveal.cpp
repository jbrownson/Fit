#include "test.hpp"
#include <boost/fit/reveal.hpp>
#include <boost/fit/conditional.hpp>
#include <boost/fit/static.hpp>
#include <boost/fit/lambda.hpp>
#include <boost/fit/fix.hpp>

namespace reveal_test {

#define CONDITIONAL_FUNCTION(n) \
struct t ## n {}; \
struct f ## n \
{ \
    constexpr int operator()(t ## n) const \
    { \
        return n; \
    } \
};

CONDITIONAL_FUNCTION(1)
CONDITIONAL_FUNCTION(2)
CONDITIONAL_FUNCTION(3)

static constexpr boost::fit::static_<boost::fit::conditional_adaptor<f1, f2, f3> > f = {}; 

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(boost::fit::reveal(f)(t1()) == 1);
    BOOST_FIT_TEST_CHECK(boost::fit::reveal(f)(t2()) == 2);
    BOOST_FIT_TEST_CHECK(boost::fit::reveal(f)(t3()) == 3);
    // boost::fit::reveal(f)(1);
}
#if BOOST_FIT_HAS_STATIC_LAMBDA
#ifndef _MSC_VER
static constexpr auto lam = boost::fit::conditional(
    BOOST_FIT_STATIC_LAMBDA(t1)
    {
        return 1;
    },
    BOOST_FIT_STATIC_LAMBDA(t2)
    {
        return 2;
    },
    BOOST_FIT_STATIC_LAMBDA(t3)
    {
        return 3;
    }
);

BOOST_FIT_TEST_CASE()
{
    STATIC_ASSERT_EMPTY(lam);
    STATIC_ASSERT_EMPTY(boost::fit::reveal(lam));
    BOOST_FIT_TEST_CHECK(boost::fit::reveal(lam)(t1()) == 1);
    BOOST_FIT_TEST_CHECK(boost::fit::reveal(lam)(t2()) == 2);
    BOOST_FIT_TEST_CHECK(boost::fit::reveal(lam)(t3()) == 3);

    // boost::fit::reveal(lam)(1);
}
#endif

BOOST_FIT_STATIC_LAMBDA_FUNCTION(static_fun) = boost::fit::conditional(
    [](t1)
    {
        return 1;
    },
    [](t2)
    {
        return 2;
    },
    [](t3)
    {
        return 3;
    }
);

BOOST_FIT_TEST_CASE()
{
#ifndef _MSC_VER
    STATIC_ASSERT_EMPTY(static_fun);
    // STATIC_ASSERT_EMPTY(boost::fit::reveal(static_fun));
#endif
    BOOST_FIT_TEST_CHECK(boost::fit::reveal(static_fun)(t1()) == 1);
    BOOST_FIT_TEST_CHECK(boost::fit::reveal(static_fun)(t2()) == 2);
    BOOST_FIT_TEST_CHECK(boost::fit::reveal(static_fun)(t3()) == 3);

    BOOST_FIT_TEST_CHECK(static_fun(t1()) == 1);
    BOOST_FIT_TEST_CHECK(static_fun(t2()) == 2);
    BOOST_FIT_TEST_CHECK(static_fun(t3()) == 3);

    // boost::fit::reveal(static_fun)(1);
}
#endif

}
