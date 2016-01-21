/*=============================================================================
    Copyright (c) 2015 Paul Fultz II
    repeat_while.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_REPEAT_WHILE_H
#define BOOST_FIT_GUARD_REPEAT_WHILE_H

/// repeat_while
/// ======
/// 
/// Description
/// -----------
/// 
/// The `repeat_while` function decorator will repeatedly apply a function while
/// the predicate returns an integral constant that is true. As such, the
/// predicate must be depedently-typed since it is never called at runtime.
/// 
/// 
/// Synopsis
/// --------
/// 
///     template<class Predicate>
///     constexpr auto repeat_while(Predicate predicate);
/// 
/// Requirements
/// ------------
/// 
/// Predicate must be:
/// 
/// * [FunctionObject](concepts.md#functionobject)
/// * MoveConstructible
/// 
/// Example
/// -------
/// 
///     struct increment
///     {
///         template<class T>
///         constexpr std::integral_constant<int, T::value + 1> operator()(T) const
///         {
///             return std::integral_constant<int, T::value + 1>();
///         }
///     };
/// 
///     struct not_6
///     {
///         template<class T>
///         constexpr std::integral_constant<bool, (T::value != 6)> 
///         operator()(T) const
///         {
///             return std::integral_constant<bool, (T::value != 6)>();
///         }
///     };
/// 
///     typedef std::integral_constant<int, 1> one;
///     typedef std::integral_constant<int, 6> six;
/// 
///     typedef decltype(boost::fit::repeat_while(not_6())(increment())(std::integral_constant<int, 1>())) increment_until_6;
/// 
/// 
///     constexpr auto increment_until_6 = boost::fit::repeat_while(not_6())(increment());
///     static_assert(std::is_same<six, decltype(increment_until_6(one()))>::value, "Error");
/// 

#include <boost/fit/always.hpp>
#include <boost/fit/detail/delegate.hpp>
#include <boost/fit/detail/result_of.hpp>
#include <boost/fit/detail/move.hpp>
#include <boost/fit/decorate.hpp>
#include <boost/fit/detail/sfinae.hpp>
#include <boost/fit/detail/static_const_var.hpp>

namespace boost { namespace fit { namespace detail {

template<class P, class... Ts>
struct compute_predicate
{
    typedef decltype(std::declval<P>()(std::declval<Ts>()...)) type;
};

template<bool B>
struct while_repeater
{
    template<class F, class P, class... Ts>
    constexpr BOOST_FIT_SFINAE_RESULT(while_repeater<
        compute_predicate<P, typename result_of<const F&, id_<Ts>...>::type>::type::value
    >, id_<const F&>, id_<const P&>, result_of<const F&, id_<Ts>...>) 
    operator()(const F& f, const P& p, Ts&&... xs) const BOOST_FIT_SFINAE_RETURNS
    (
        while_repeater<
            compute_predicate<P, decltype(f(boost::fit::forward<Ts>(xs)...))>::type::value
        >()(f, p, f(boost::fit::forward<Ts>(xs)...))
    );
};

template<>
struct while_repeater<false>
{
    template<class F, class P, class T>
    constexpr T operator()(const F&, const P&, T&& x) const
    {
        return x;
    }
};

struct repeat_while_decorator
{
    template<class P, class F, class... Ts>
    constexpr auto operator()(const P& p, const F& f, Ts&&... xs) const BOOST_FIT_RETURNS
    (
        detail::while_repeater<
            detail::compute_predicate<P, decltype(std::declval<F>()(boost::fit::forward<Ts>(xs)...))>::type::value
        >()
        (
            f, 
            p, 
            boost::fit::forward<Ts>(xs)...
        )
    );
};

}

BOOST_FIT_DECLARE_STATIC_VAR(repeat_while, decorate_adaptor<detail::repeat_while_decorator>);

}} // namespace boost::fit

#endif
