/*=============================================================================
    Copyright (c) 2014 Paul Fultz II
    placeholders.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_FUNCTION_PLACEHOLDERS_H
#define BOOST_FIT_GUARD_FUNCTION_PLACEHOLDERS_H

/// placeholders
/// ============
/// 
/// Description
/// -----------
/// 
/// The placeholders provide `std::bind` compatible placeholders, that
/// additionally provide basic C++ operators that creates bind expressions.
/// Each bind expression supports `constexpr` function evaluation.
/// 
/// Synopsis
/// --------
/// 
///     placeholder<1> _1 = {};
///     placeholder<2> _2 = {};
///     placeholder<3> _3 = {};
///     placeholder<4> _4 = {};
///     placeholder<5> _5 = {};
///     placeholder<6> _6 = {};
///     placeholder<7> _7 = {};
///     placeholder<8> _8 = {};
///     placeholder<9> _9 = {};
/// 
/// Operators
/// ---------
/// 
/// * Binary operators: +,-,*,/,%,>>,<<,>,<,<=,>=,==,!=,&,^,|,&&,||
/// * Assign operators: +=,-=,*=,/=,%=,>>=,<<=,&=,|=,^=
/// * Unary operators: !,~,+,-,*,++,--
/// 
/// 
/// Example
/// -------
/// 
///     auto sum = _1 + _2;
///     assert(3 == sum(1, 2));
/// 
/// 
/// unamed placeholder
/// ==================
/// 
/// Description
/// -----------
/// 
/// The unamed placeholder can be used to build simple functions from C++
/// operators. 
/// 
/// Note: The function produced by the unamed placeholder is not a bind expression.
/// 
/// Synopsis
/// --------
/// 
///     /* unspecified */ _ = {};
/// 
/// Example
/// -------
/// 
///     auto sum = _ + _;
///     assert(3 == sum(1, 2));
/// 

#include <boost/fit/lazy.hpp>
#include <boost/fit/protect.hpp>

namespace boost { namespace fit { namespace detail {
    template<int N>
    struct simple_placeholder
    {};
}}} // namespace boost::fit

namespace std {
    template<int N>
    struct is_placeholder<boost::fit::detail::simple_placeholder<N>>
    : std::integral_constant<int, N>
    {};
}


namespace boost { namespace fit {

#define BOOST_FIT_FOREACH_BINARY_OP(m) \
    m(+, add) \
    m(-, subtract) \
    m(*, multiply) \
    m(/, divide) \
    m(%, remainder) \
    m(>>, shift_right) \
    m(<<, shift_left) \
    m(>, greater_than) \
    m(<, less_than) \
    m(<=, less_than_equal) \
    m(>=, greater_than_equal) \
    m(==, equal) \
    m(!=, not_equal) \
    m(&, bit_and) \
    m(^, xor_) \
    m(|, bit_or) \
    m(&&, and_) \
    m(||, or_)

#define BOOST_FIT_FOREACH_ASSIGN_OP(m) \
    m(+=, assign_add) \
    m(-=, assign_subtract) \
    m(*=, assign_multiply) \
    m(/=, assign_divide) \
    m(%=, assign_remainder) \
    m(>>=, assign_right_shift) \
    m(<<=, assign_left_shift) \
    m(&=, assign_bit_and) \
    m(|=, assign_bit_or) \
    m(^=, assign_xor)

#ifndef _MSC_VER
#define BOOST_FIT_FOREACH_UNARY_OP(m) \
    m(!, not_) \
    m(~, compl_) \
    m(+, unary_plus) \
    m(-, unary_subtract) \
    m(*, dereference) \
    m(++, increment) \
    m(--, decrement)
#else
#define BOOST_FIT_FOREACH_UNARY_OP(m) \
    m(!, not_) \
    m(~, compl_) \
    m(+, unary_plus) \
    m(-, unary_subtract) \
    m(*, dereference)
#endif

namespace operators {

struct call
{
    template<class F, class... Ts>
    constexpr auto operator()(F&& f, Ts&&... xs) const BOOST_FIT_RETURNS
    (f(boost::fit::forward<Ts>(xs)...));
};

#define BOOST_FIT_BINARY_OP(op, name) \
    struct name \
    { \
        template<class T, class U> \
        constexpr auto operator()(T&& x, U&& y) const BOOST_FIT_RETURNS \
        (boost::fit::forward<T>(x) op boost::fit::forward<U>(y)); \
    };

BOOST_FIT_FOREACH_BINARY_OP(BOOST_FIT_BINARY_OP)
BOOST_FIT_FOREACH_ASSIGN_OP(BOOST_FIT_BINARY_OP)

#define BOOST_FIT_UNARY_OP(op, name) \
    struct name \
    { \
        template<class T> \
        constexpr auto operator()(T&& x) const BOOST_FIT_RETURNS \
        (op(boost::fit::forward<T>(x))); \
    };


BOOST_FIT_FOREACH_UNARY_OP(BOOST_FIT_UNARY_OP)

}

template<int N>
struct placeholder
{
#if BOOST_FIT_HAS_MANGLE_OVERLOAD
    template<class... Ts>
    constexpr auto operator()(Ts&&... xs) const BOOST_FIT_RETURNS 
    ( boost::fit::lazy(operators::call())(detail::simple_placeholder<N>(), boost::fit::forward<Ts>(xs)...) );
#else
    template<class... Ts>
    struct result_call
    { typedef decltype(boost::fit::lazy(operators::call())(detail::simple_placeholder<N>(), std::declval<Ts>()...)) type; };
    template<class... Ts>
    constexpr typename result_call<Ts...>::type operator()(Ts&&... xs) const 
    { return boost::fit::lazy(operators::call())(detail::simple_placeholder<N>(), boost::fit::forward<Ts>(xs)...); };

#endif

#define BOOST_FIT_PLACEHOLDER_UNARY_OP(op, name) \
    constexpr auto operator op () const BOOST_FIT_RETURNS \
    ( boost::fit::lazy(operators::name())(detail::simple_placeholder<N>()) );

BOOST_FIT_FOREACH_UNARY_OP(BOOST_FIT_PLACEHOLDER_UNARY_OP)

#define BOOST_FIT_PLACEHOLDER_ASSIGN_OP(op, name) \
    template<class T> \
    constexpr auto operator op (T&& x) const BOOST_FIT_RETURNS \
    ( boost::fit::lazy(operators::name())(detail::simple_placeholder<N>(), boost::fit::forward<T>(x)) );

BOOST_FIT_FOREACH_ASSIGN_OP(BOOST_FIT_PLACEHOLDER_ASSIGN_OP)

};

#if BOOST_FIT_HAS_MANGLE_OVERLOAD

#define BOOST_FIT_PLACEHOLDER_BINARY_OP(op, name) \
    template<class T, int N> \
    constexpr inline auto operator op (const placeholder<N>&, T&& x) BOOST_FIT_RETURNS \
    ( boost::fit::lazy(operators::name())(detail::simple_placeholder<N>(), boost::fit::forward<T>(x)) ); \
    template<class T, int N> \
    constexpr inline auto operator op (T&& x, const placeholder<N>&) BOOST_FIT_RETURNS \
    ( boost::fit::lazy(operators::name())(boost::fit::forward<T>(x), detail::simple_placeholder<N>()) ); \
    template<int N, int M> \
    constexpr inline auto operator op (const placeholder<N>&, const placeholder<M>&) BOOST_FIT_RETURNS \
    ( boost::fit::lazy(operators::name())(detail::simple_placeholder<N>(), detail::simple_placeholder<M>()) );

#else

#define BOOST_FIT_PLACEHOLDER_BINARY_OP(op, name) \
    template<class T, class U> \
    struct result_ ## name \
    { typedef decltype(boost::fit::lazy(operators::name())(std::declval<T>(), std::declval<U>())) type; }; \
    template<class T, int N> \
    constexpr inline typename result_ ## name<detail::simple_placeholder<N>, T>::type operator op (const placeholder<N>&, T&& x) \
    { return boost::fit::lazy(operators::name())(detail::simple_placeholder<N>(), boost::fit::forward<T>(x)); } \
    template<class T, int N> \
    constexpr inline typename result_ ## name<T, detail::simple_placeholder<N>>::type operator op (T&& x, const placeholder<N>&) \
    { return boost::fit::lazy(operators::name())(boost::fit::forward<T>(x), detail::simple_placeholder<N>()); } \
    template<int N, int M> \
    constexpr inline typename result_ ## name<detail::simple_placeholder<N>, detail::simple_placeholder<M>>::type operator op (const placeholder<N>&, const placeholder<M>&) \
    { return boost::fit::lazy(operators::name())(detail::simple_placeholder<N>(), detail::simple_placeholder<M>()); }

#endif

BOOST_FIT_FOREACH_BINARY_OP(BOOST_FIT_PLACEHOLDER_BINARY_OP)

BOOST_FIT_DECLARE_STATIC_VAR(_1, placeholder<1>);
BOOST_FIT_DECLARE_STATIC_VAR(_2, placeholder<2>);
BOOST_FIT_DECLARE_STATIC_VAR(_3, placeholder<3>);
BOOST_FIT_DECLARE_STATIC_VAR(_4, placeholder<4>);
BOOST_FIT_DECLARE_STATIC_VAR(_5, placeholder<5>);
BOOST_FIT_DECLARE_STATIC_VAR(_6, placeholder<6>);
BOOST_FIT_DECLARE_STATIC_VAR(_7, placeholder<7>);
BOOST_FIT_DECLARE_STATIC_VAR(_8, placeholder<8>);
BOOST_FIT_DECLARE_STATIC_VAR(_9, placeholder<9>);


namespace detail {



struct unamed_placeholder
{
template<class T, class Invoker>
struct partial_ap
{
    T val;

    BOOST_FIT_INHERIT_DEFAULT_EMPTY(partial_ap, T)

    template<class X, class... Xs, BOOST_FIT_ENABLE_IF_CONSTRUCTIBLE(T, X&&, Xs&&...)>
    constexpr partial_ap(X&& x, Xs&&... xs) : val(boost::fit::forward<X>(x), boost::fit::forward<Xs>(xs)...)
    {}

    BOOST_FIT_RETURNS_CLASS(partial_ap);

    template<class X>
    constexpr auto operator()(X&& x) const BOOST_FIT_RETURNS
    (Invoker()(BOOST_FIT_CONST_THIS->val, boost::fit::forward<X>(x)));
};

template<class Op>
struct left
{
    template<class T, class X>
    constexpr auto operator()(const T& val, X&& x) const BOOST_FIT_RETURNS
    (Op()(val, boost::fit::forward<X>(x)));
};

template<class Op>
struct right
{
    template<class T, class X>
    constexpr auto operator()(const T& val, X&& x) const BOOST_FIT_RETURNS
    (Op()(boost::fit::forward<X>(x), val));
};

#define BOOST_FIT_UNAMED_PLACEHOLDER_UNARY_OP(op, name) \
    constexpr auto operator op () const BOOST_FIT_RETURNS \
    ( operators::name() );

BOOST_FIT_FOREACH_UNARY_OP(BOOST_FIT_UNAMED_PLACEHOLDER_UNARY_OP)

#define BOOST_FIT_UNAMED_PLACEHOLDER_ASSIGN_OP(op, name) \
    template<class T> \
    constexpr auto operator op (const T& x) const BOOST_FIT_RETURNS \
    ( partial_ap<T, left<operators::name>>(x) );

BOOST_FIT_FOREACH_ASSIGN_OP(BOOST_FIT_UNAMED_PLACEHOLDER_ASSIGN_OP)
};
#define BOOST_FIT_UNAMED_PLACEHOLDER_BINARY_OP(op, name) \
    template<class T> \
    constexpr inline auto operator op (const unamed_placeholder&, const T& x) BOOST_FIT_RETURNS \
    ( unamed_placeholder::partial_ap<T, unamed_placeholder::right<operators::name>>(x) ); \
    template<class T> \
    constexpr inline auto operator op (const T& x, const unamed_placeholder&) BOOST_FIT_RETURNS \
    ( unamed_placeholder::partial_ap<T, unamed_placeholder::left<operators::name>>(x) ); \
    constexpr inline auto operator op (const unamed_placeholder&, const unamed_placeholder&) BOOST_FIT_RETURNS \
    ( operators::name() );

BOOST_FIT_FOREACH_BINARY_OP(BOOST_FIT_UNAMED_PLACEHOLDER_BINARY_OP)
}

BOOST_FIT_DECLARE_STATIC_VAR(_, detail::unamed_placeholder);

}} // namespace boost::fit

namespace std {
    template<int N>
    struct is_placeholder<boost::fit::placeholder<N>>
    : std::integral_constant<int, N>
    {};
}

#endif
