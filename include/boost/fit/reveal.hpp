/*=============================================================================
    Copyright (c) 2014 Paul Fultz II
    reveal.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_FUNCTION_REVEAL_H
#define BOOST_FIT_GUARD_FUNCTION_REVEAL_H

/// reveal
/// ======
/// 
/// Description
/// -----------
/// 
/// The `reveal` function adaptor helps shows the error messages that get
/// masked on some compilers. Sometimes an error in a function that causes a
/// substitution failure, will remove the function from valid overloads. On
/// compilers without a backtrace for substitution failure, this will mask the
/// error inside the function. The `reveal` adaptor will expose these error
/// messages while still keeping the function SFINAE-friendly.
/// 
/// Synopsis
/// --------
/// 
///     template<class F>
///     reveal_adaptor<F> reveal(F f);
/// 
/// Requirements
/// ------------
/// 
/// F must be:
/// 
/// * [Callable](concepts.md#callable)
/// * MoveConstructible
/// 

#include <boost/fit/always.hpp>
#include <boost/fit/returns.hpp>
#include <boost/fit/is_callable.hpp>
#include <boost/fit/identity.hpp>
#include <boost/fit/detail/move.hpp>
#include <boost/fit/detail/callable_base.hpp>
#include <boost/fit/detail/delegate.hpp>
#include <boost/fit/detail/holder.hpp>
#include <boost/fit/detail/join.hpp>
#include <boost/fit/detail/make.hpp>
#include <boost/fit/detail/static_const_var.hpp>

#ifndef BOOST_FIT_HAS_TEMPLATE_ALIAS
#if defined(__GNUC__) && !defined (__clang__) && __GNUC__ == 4 && __GNUC_MINOR__ < 7
#define BOOST_FIT_HAS_TEMPLATE_ALIAS 0
#else
#define BOOST_FIT_HAS_TEMPLATE_ALIAS 1
#endif
#endif

namespace boost { namespace fit { 

namespace detail {
template<template<class...> class T>
struct template_holder
{
    typedef void type;
};

template<class T, class=void>
struct has_failure
: std::false_type
{};

template<class T>
struct has_failure<T, typename holder<
    typename T::failure
>::type>
: std::true_type
{};
}

template<class F, class=void>
struct get_failure
{
    template<class... Ts>
    struct of
    {
#if BOOST_FIT_HAS_TEMPLATE_ALIAS
        template<class Id>
        using apply = decltype(Id()(std::declval<F>())(std::declval<Ts>()...));
#else
        template<class Id>
        static auto apply(Id id) -> decltype(id(std::declval<F>())(std::declval<Ts>()...));
#endif
    };
};

template<class F>
struct get_failure<F, typename std::enable_if<detail::has_failure<F>::value>::type>
: F::failure
{};

namespace detail {
template<class Failure, class... Ts>
struct apply_failure
: Failure::template of<Ts...>
{};

template<class F, class Failure>
struct reveal_failure
{
    // Add default constructor to make clang 3.4 happy
    constexpr reveal_failure()
    {}
    // This is just a placeholder to produce a note in the compiler, it is
    // never called
    template<
        class... Ts, 
        class=typename std::enable_if<(!is_callable<F, Ts...>::value)>::type
    >
    constexpr auto operator()(Ts&&... xs) const -> 
#if BOOST_FIT_HAS_TEMPLATE_ALIAS
        typename apply_failure<Failure, Ts...>::template apply<boost::fit::detail::identity_base>;
#else
        decltype(apply_failure<Failure, Ts...>::apply(identity));
#endif
};

template<class F, class Failure=get_failure<F>, class=void>
struct traverse_failure 
: reveal_failure<F, Failure>
{
    constexpr traverse_failure()
    {}
};

template<class F, class Failure>
struct traverse_failure<F, Failure, typename holder< 
    typename Failure::children
>::type> 
: Failure::children::template overloads<F>
{
    constexpr traverse_failure()
    {}
};

template<class Failure, class Transform, class=void>
struct transform_failures 
: Transform::template apply<Failure>
{};

template<class Failure, class Transform>
struct transform_failures<Failure, Transform, typename holder< 
    typename Failure::children
>::type> 
: Failure::children::template transform<Transform>
{};

}

template<class Failure, class... Failures>
struct failures;

template<class... Fs>
struct with_failures
{
    typedef BOOST_FIT_JOIN(failures, Fs...) children;
};

template<class Failure, class... Failures>
struct failures 
{
    template<class Transform>
    struct transform
    : with_failures<detail::transform_failures<Failure, Transform>, detail::transform_failures<Failures, Transform>...>
    {};
    template<class F, class FailureBase=BOOST_FIT_JOIN(failures, Failures...)>
    struct overloads
    : detail::traverse_failure<F, Failure>, FailureBase::template overloads<F>
    {
        constexpr overloads()
        {}
        using detail::traverse_failure<F, Failure>::operator();
        using FailureBase::template overloads<F>::operator();
    };
};

template<class Failure>
struct failures<Failure>
{
    template<class Transform>
    struct transform
    : with_failures<detail::transform_failures<Failure, Transform>>
    {};
    template<class F>
    struct overloads
    : detail::traverse_failure<F, Failure>
    {
        constexpr overloads()
        {}
    };
};

template<class Transform, class... Fs>
struct failure_map
: with_failures<detail::transform_failures<get_failure<Fs>, Transform>...>
{};

template<class... Fs>
struct failure_for
: with_failures<get_failure<Fs>...>
{};

template<class F>
struct reveal_adaptor
: detail::traverse_failure<detail::callable_base<F>>, detail::callable_base<F>
{
    typedef reveal_adaptor fit_rewritable1_tag;
    using detail::traverse_failure<detail::callable_base<F>>::operator();
    using detail::callable_base<F>::operator();

    BOOST_FIT_INHERIT_CONSTRUCTOR(reveal_adaptor, detail::callable_base<F>);
};
// Avoid double reveals, it causes problem on gcc 4.6
template<class F>
struct reveal_adaptor<reveal_adaptor<F>>
: reveal_adaptor<F>
{
    typedef reveal_adaptor fit_rewritable1_tag;
    BOOST_FIT_INHERIT_CONSTRUCTOR(reveal_adaptor, reveal_adaptor<F>);
};

BOOST_FIT_DECLARE_STATIC_VAR(reveal, detail::make<reveal_adaptor>);

}} // namespace boost::fit

#endif
