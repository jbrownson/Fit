/*=============================================================================
    Copyright (c) 2016 Paul Fultz II
    limit.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_LIMIT_H
#define BOOST_FIT_GUARD_LIMIT_H

/// limit
/// =====
/// 
/// Description
/// -----------
/// 
/// The `limit` function decorator annotates the function with the max number of
/// parameters. The `limit_c` version can be used to give a the number
/// directly(instead of relying on dependent typing).
/// 
/// Synopsis
/// --------
/// 
///     template<class IntegralConstant>
///     constexpr auto limit(IntegralConstant);
/// 
///     template<std::size_t N, class F>
///     constexpr auto limit_c(F);
/// 
/// Requirements
/// ------------
/// 
/// IntegralConstant must be:
/// 
/// * IntegralConstant
/// 
/// F must be:
/// 
/// * [Callable](concepts.md#callable)
/// * MoveConstructible
/// 
/// Example
/// -------
/// 
///     struct sum_f
///     {
///         template<class T>
///         int operator()(T x, T y) const
///         {
///             return x+y
///         }
///     };
///     BOOST_FIT_STATIC_FUNCTION(sum) = limit_c<2>(sum_f());
/// 
/// function_param_limit
/// ====================
/// 
/// Description
/// -----------
/// 
/// The `function_param_limit` metafunction retrieves the maxium number of
/// parameters for a function.
/// 
/// Synopsis
/// --------
/// 
///     template<class F>
///     struct function_param_limit;
/// 

#include <boost/fit/detail/callable_base.hpp>
#include <boost/fit/detail/forward.hpp>
#include <boost/fit/detail/delegate.hpp>
#include <boost/fit/detail/move.hpp>
#include <boost/fit/detail/static_const_var.hpp>
#include <cstdint>

namespace boost { namespace fit {

namespace detail {

template<std::size_t N, class F>
struct limit_adaptor : detail::callable_base<F>
{
    typedef std::integral_constant<std::size_t, N> fit_function_param_limit;
    BOOST_FIT_INHERIT_CONSTRUCTOR(limit_adaptor, detail::callable_base<F>)
};

template<std::size_t N>
struct make_limit_f
{
    constexpr make_limit_f()
    {}
    template<class F>
    constexpr limit_adaptor<N, F> operator()(F f) const
    {
        return limit_adaptor<N, F>(boost::fit::move(f));
    }
};

struct limit_f
{
    template<class IntegralConstant, std::size_t N=IntegralConstant::type::value>
    constexpr make_limit_f<N> operator()(IntegralConstant) const
    {
        return {};
    }
};

}

template<std::size_t N, class F>
constexpr detail::limit_adaptor<N, F> limit_c(F f)
{
    return detail::limit_adaptor<N, F>(boost::fit::move(f));
}

BOOST_FIT_DECLARE_STATIC_VAR(limit, detail::limit_f);


template<class F, class=void>
struct function_param_limit
: std::integral_constant<std::size_t, SIZE_MAX>
{};

template<class F>
struct function_param_limit<F, typename detail::holder<typename F::fit_function_param_limit>::type>
: F::fit_function_param_limit
{};

}} // namespace boost::fit

#endif
