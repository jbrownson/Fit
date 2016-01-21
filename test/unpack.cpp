#include <boost/fit/unpack.hpp>
#include <boost/fit/static.hpp>
#include <boost/fit/lambda.hpp>
#include "test.hpp"

#include <memory>

static constexpr boost::fit::static_<boost::fit::unpack_adaptor<unary_class> > unary_unpack = {};
static constexpr boost::fit::static_<boost::fit::unpack_adaptor<binary_class> > binary_unpack = {};

BOOST_FIT_STATIC_AUTO unary_unpack_constexpr = boost::fit::unpack_adaptor<unary_class>();
BOOST_FIT_STATIC_AUTO binary_unpack_constexpr = boost::fit::unpack_adaptor<binary_class>();

BOOST_FIT_STATIC_AUTO unary_unpack_reveal = boost::fit::reveal_adaptor<boost::fit::unpack_adaptor<unary_class>>();
BOOST_FIT_STATIC_AUTO binary_unpack_reveal = boost::fit::reveal_adaptor<boost::fit::unpack_adaptor<binary_class>>();

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(unary_class())(std::make_tuple(3)));
    BOOST_FIT_TEST_CHECK(3 == unary_unpack(std::make_tuple(3)));
    BOOST_FIT_TEST_CHECK(3 == unary_unpack_reveal(std::make_tuple(3)));
    int ifu = 3;
    BOOST_FIT_TEST_CHECK(3 == unary_unpack(std::tuple<int&>(ifu)));

    BOOST_FIT_STATIC_TEST_CHECK(3 == boost::fit::unpack(unary_class())(std::make_tuple(3)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == unary_unpack_constexpr(std::make_tuple(3)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == unary_unpack_reveal(std::make_tuple(3)));
}

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(unary_class())(boost::fit::pack_decay(3)));
    BOOST_FIT_TEST_CHECK(3 == unary_unpack(boost::fit::pack_decay(3)));
    BOOST_FIT_TEST_CHECK(3 == unary_unpack_reveal(boost::fit::pack_decay(3)));
    int ifu = 3;
    BOOST_FIT_TEST_CHECK(3 == unary_unpack(boost::fit::pack_forward(ifu)));

    BOOST_FIT_STATIC_TEST_CHECK(3 == boost::fit::unpack(unary_class())(boost::fit::pack_decay(3)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == unary_unpack_constexpr(boost::fit::pack_decay(3)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == unary_unpack_reveal(boost::fit::pack_decay(3)));
}

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(1, 2)));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack(std::make_tuple(1, 2)));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(1, 2)));

    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(1), std::make_tuple(2)));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack(std::make_tuple(1), std::make_tuple(2)));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(1), std::make_tuple(2)));

    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack(std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));

    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(), std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack(std::make_tuple(), std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(), std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));

    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(1), std::make_tuple(), std::make_tuple(2), std::make_tuple()));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack(std::make_tuple(1), std::make_tuple(), std::make_tuple(2), std::make_tuple()));
    BOOST_FIT_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(1), std::make_tuple(), std::make_tuple(2), std::make_tuple()));

    BOOST_FIT_STATIC_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(1, 2)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_constexpr(std::make_tuple(1, 2)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(1, 2)));

    BOOST_FIT_STATIC_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(1), std::make_tuple(2)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_constexpr(std::make_tuple(1), std::make_tuple(2)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(1), std::make_tuple(2)));

    BOOST_FIT_STATIC_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_constexpr(std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));

    BOOST_FIT_STATIC_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(), std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_constexpr(std::make_tuple(), std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(), std::make_tuple(1), std::make_tuple(), std::make_tuple(2)));

    BOOST_FIT_STATIC_TEST_CHECK(3 == boost::fit::unpack(binary_class())(std::make_tuple(1), std::make_tuple(), std::make_tuple(2), std::make_tuple()));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_constexpr(std::make_tuple(1), std::make_tuple(), std::make_tuple(2), std::make_tuple()));
    BOOST_FIT_STATIC_TEST_CHECK(3 == binary_unpack_reveal(std::make_tuple(1), std::make_tuple(), std::make_tuple(2), std::make_tuple()));
}

BOOST_FIT_TEST_CASE()
{
    auto p1 = boost::fit::pack(1, 2);
    static_assert(boost::fit::is_unpackable<decltype(p1)>::value, "Not unpackable");
    static_assert(boost::fit::is_unpackable<decltype((p1))>::value, "Not unpackable");

    auto p2 = boost::fit::pack_forward(1, 2);
    static_assert(boost::fit::is_unpackable<decltype(p2)>::value, "Not unpackable");
    static_assert(boost::fit::is_unpackable<decltype((p2))>::value, "Not unpackable");

    auto p3 = boost::fit::pack_decay(1, 2);
    static_assert(boost::fit::is_unpackable<decltype(p3)>::value, "Not unpackable");
    static_assert(boost::fit::is_unpackable<decltype((p3))>::value, "Not unpackable");

    static_assert(boost::fit::is_unpackable<std::tuple<int>>::value, "Not unpackable");
    
    static_assert(!boost::fit::is_unpackable<int>::value, "Unpackable");
    static_assert(!boost::fit::is_unpackable<void>::value, "Unpackable");
}
#if BOOST_FIT_HAS_STATIC_LAMBDA
BOOST_FIT_STATIC_AUTO lambda_unary_unpack = boost::fit::unpack(BOOST_FIT_STATIC_LAMBDA(int x)
{
    return x;
});

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(3 == lambda_unary_unpack(std::make_tuple(3)));
}

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(3 == lambda_unary_unpack(boost::fit::pack_decay(3)));
}
#endif
struct unary_move
{
    std::unique_ptr<int> i;
    unary_move()
    : i(new int(2))
    {}

    template<class T>
    T operator()(T x) const
    {
        return x + *i;
    }
};

static constexpr boost::fit::static_<boost::fit::unpack_adaptor<unary_move> > unary_move_unpack = {};

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(unary_move())(std::make_tuple(1)));
    BOOST_FIT_TEST_CHECK(3 == unary_move_unpack(std::make_tuple(1)));
}

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(unary_move())(boost::fit::pack_decay(1)));
    BOOST_FIT_TEST_CHECK(3 == unary_move_unpack(boost::fit::pack_decay(1)));
}

struct indirect_sum_f
{
    template<class T, class U>
    auto operator()(T x, U y) const
    BOOST_FIT_RETURNS(*x + *y);
};

#define MAKE_UNIQUE_PTR(x) std::unique_ptr<int>(new int(x)) 

BOOST_FIT_TEST_CASE()
{
    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(indirect_sum_f())(boost::fit::pack(MAKE_UNIQUE_PTR(1), MAKE_UNIQUE_PTR(2))));
    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(indirect_sum_f())(boost::fit::pack_forward(MAKE_UNIQUE_PTR(1), MAKE_UNIQUE_PTR(2))));
    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(indirect_sum_f())(boost::fit::pack_decay(MAKE_UNIQUE_PTR(1), MAKE_UNIQUE_PTR(2))));
    BOOST_FIT_TEST_CHECK(3 == boost::fit::unpack(indirect_sum_f())(std::make_tuple(MAKE_UNIQUE_PTR(1), MAKE_UNIQUE_PTR(2))));
}

template<class...>
struct deduce_types
{};

struct deducer
{
    template<class... Ts>
    deduce_types<Ts...> operator()(Ts&&...) const;
};

static constexpr boost::fit::unpack_adaptor<deducer> deduce = {};


BOOST_FIT_TEST_CASE()
{
    STATIC_ASSERT_SAME(deduce_types<int, int>, decltype(deduce(std::make_tuple(1, 2))));
    STATIC_ASSERT_SAME(deduce_types<int, int>, decltype(deduce(std::make_tuple(1), std::make_tuple(2))));
    STATIC_ASSERT_SAME(deduce_types<int, int, int>, decltype(deduce(std::make_tuple(1), std::make_tuple(2), std::make_tuple(3))));
    STATIC_ASSERT_SAME(std::tuple<int&&, int&&>, decltype(std::forward_as_tuple(1, 2)));
    // Disable this test, it seems that rvalue references get swalllowed by type deduction
    // STATIC_ASSERT_SAME(deduce_types<int&&, int&&>, decltype(deduce(std::forward_as_tuple(1, 2))));


    STATIC_ASSERT_SAME(deduce_types<int, int>, decltype(deduce(boost::fit::pack(1, 2))));
    STATIC_ASSERT_SAME(deduce_types<int, int>, decltype(deduce(boost::fit::pack(1), boost::fit::pack(2))));
    STATIC_ASSERT_SAME(deduce_types<int, int, int>, decltype(deduce(boost::fit::pack(1), boost::fit::pack(2), boost::fit::pack(3))));
    // STATIC_ASSERT_SAME(deduce_types<int&&, int&&>, decltype(deduce(boost::fit::pack_forward(1, 2))));
}
