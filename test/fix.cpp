#include <boost/fit/fix.hpp>
#include <boost/fit/static.hpp>
#include <boost/fit/reveal.hpp>
#include "test.hpp"

#include <memory>

struct factorial_t
{
    template<class Self, class T>
    BOOST_FIT_FIX_CONSTEXPR T operator()(Self s, T x) const
    {
        return x == 0 ? 1 : x * s(x-1);
    }
};

struct factorial_move_t
{
    std::unique_ptr<int> i;
    factorial_move_t() : i(new int(1))
    {}
    template<class Self, class T>
    BOOST_FIT_FIX_CONSTEXPR T operator()(const Self& s, T x) const
    {
        return x == 0 ? *i : x * s(x-1);
    }
};

static constexpr boost::fit::fix_adaptor<factorial_t> factorial = {};
static constexpr boost::fit::static_<boost::fit::fix_adaptor<factorial_move_t> > factorial_move = {};

BOOST_FIT_TEST_CASE()
{
    const int r1 = factorial(5);
    const int r2 = boost::fit::reveal(factorial)(5);
    BOOST_FIT_TEST_CHECK(r1 == 5*4*3*2*1);
    BOOST_FIT_TEST_CHECK(r2 == 5*4*3*2*1);
#if BOOST_FIT_FIX_HAS_CONSTEXPR
    BOOST_FIT_STATIC_TEST_CHECK(r1 == 5*4*3*2*1);
#endif
#if BOOST_FIT_HAS_GENERIC_LAMBDA
    int r3 = boost::fit::fix([](auto s, auto x) -> decltype(x) { return x == 0 ? 1 : x * s(x-1); })(5);
    BOOST_FIT_TEST_CHECK(r3 == 5*4*3*2*1);
#endif
}

BOOST_FIT_TEST_CASE()
{
    const int r = factorial_move(5);
    BOOST_FIT_TEST_CHECK(r == 5*4*3*2*1);
    BOOST_FIT_TEST_CHECK(boost::fit::fix(factorial_move_t())(5) == 5*4*3*2*1);
}
