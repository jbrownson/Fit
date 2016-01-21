#include <boost/fit/flow.hpp>
#include <memory>
#include "test.hpp"

namespace flow_test {
struct increment
{
    template<class T>
    constexpr T operator()(T x) const
    {
        return x + 1;
    }
};

struct decrement
{
    template<class T>
    constexpr T operator()(T x) const
    {
        return x - 1;
    }
};

struct negate
{
    template<class T>
    constexpr T operator()(T x) const
    {
        return -x;
    }
};

struct increment_movable
{
    std::unique_ptr<int> n;
    increment_movable()
    : n(new int(1))
    {}
    template<class T>
    T operator()(T x) const
    {
        return x + *n;
    }
};

struct decrement_movable
{
    std::unique_ptr<int> n;
    decrement_movable()
    : n(new int(1))
    {}
    template<class T>
    T operator()(T x) const
    {
        return x - *n;
    }
};

BOOST_FIT_TEST_CASE()
{
    int r = boost::fit::flow(increment(), decrement(), increment())(3);
    BOOST_FIT_TEST_CHECK(r == 4);
    BOOST_FIT_STATIC_TEST_CHECK(boost::fit::flow(increment(), decrement(), increment())(3) == 4);
}

BOOST_FIT_TEST_CASE()
{
    int r = boost::fit::flow(increment(), negate(), decrement(), decrement())(3);
    BOOST_FIT_TEST_CHECK(r == -6);
    BOOST_FIT_STATIC_TEST_CHECK(boost::fit::flow(increment(), negate(), decrement(), decrement())(3) == -6);
}
#ifndef _MSC_VER
BOOST_FIT_TEST_CASE()
{
    constexpr auto f = boost::fit::flow(increment(), decrement());
    static_assert(std::is_empty<decltype(f)>::value, "Compose function not empty");
    int r = f(3);
    BOOST_FIT_TEST_CHECK(r == 3);
    BOOST_FIT_STATIC_TEST_CHECK(f(3) == 3);
}
#endif

BOOST_FIT_TEST_CASE()
{
    STATIC_ASSERT_MOVE_ONLY(increment_movable);
    STATIC_ASSERT_MOVE_ONLY(decrement_movable);
    int r = boost::fit::flow(increment_movable(), decrement_movable(), increment_movable())(3);
    BOOST_FIT_TEST_CHECK(r == 4);
}

BOOST_FIT_TEST_CASE()
{
    const auto f = boost::fit::flow([](int i) { return i+1; }, [](int i) { return i-1; }, [](int i) { return i+1; });
#ifndef _MSC_VER
    static_assert(std::is_empty<decltype(f)>::value, "Compose function not empty");
#endif
    int r = f(3);
    BOOST_FIT_TEST_CHECK(r == 4);
}
}
