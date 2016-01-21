/*=============================================================================
    Copyright (c) 2014 Paul Fultz II
    protect.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_FUNCTION_PROTECT_H
#define BOOST_FIT_GUARD_FUNCTION_PROTECT_H

/// protect
/// =======
/// 
/// Description
/// -----------
/// 
/// The `protect` function adaptor can be used to make a bind expression be
/// treated as a normal function instead. Both `bind` and [`lazy`](lazy.md)
/// eargerly evaluates nested bind expressions. The `protect` adaptor masks
/// the type so `bind` or [`lazy`](lazy.md) no longer recognizes the function
/// as bind expression and evaluates it.
/// 
/// Synopsis
/// --------
/// 
///     template<class F>
///     constexpr protect_adaptor<F> protect(F f);
/// 
/// Requirements
/// ------------
/// 
/// F must be:
/// 
/// * [Callable](concepts.md#callable)
/// * MoveConstructible
/// 

#include <utility>
#include <boost/fit/reveal.hpp>
#include <boost/fit/detail/forward.hpp>
#include <boost/fit/detail/make.hpp>
#include <boost/fit/detail/static_const_var.hpp>

namespace boost { namespace fit {

template<class F>
struct protect_adaptor : detail::callable_base<F>
{
    typedef protect_adaptor fit_rewritable1_tag;
    template<class... Ts>
    constexpr protect_adaptor(Ts&&... xs) : detail::callable_base<F>(boost::fit::forward<Ts>(xs)...)
    {}
};

BOOST_FIT_DECLARE_STATIC_VAR(protect, detail::make<protect_adaptor>);

}} // namespace boost::fit
#endif
