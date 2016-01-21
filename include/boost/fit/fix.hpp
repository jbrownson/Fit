/*=============================================================================
    Copyright (c) 2014 Paul Fultz II
    fix.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_FUNCTION_FIX_H
#define BOOST_FIT_GUARD_FUNCTION_FIX_H

/// fix
/// ===
/// 
/// Description
/// -----------
/// 
/// The `fix` function adaptor implements a fixed-point combinator. This can be
/// used to write recursive functions. 
/// 
/// Note: Compilers are too eager to instantiate templates when using
/// constexpr, which causes the compiler to reach its internal instantiation
/// limit. So, unfortunately, `fix` cannot be used for `constexpr` functions.
/// 
/// Synopsis
/// --------
/// 
///     template<class F>
///     constexpr fix_adaptor<F> fix(F f);
/// 
/// Semantics
/// ---------
/// 
///     assert(fix(f)(xs...) == f(f, xs...));
/// 
/// Requirements
/// ------------
/// 
/// F must be:
/// 
/// * [Callable](concepts.md#callable)
/// * MoveConstructible
/// 
/// Example
/// -------
/// 
///     int r = boost::fit::fix([](auto s, auto x) -> decltype(x) { return x == 0 ? 1 : x * s(x-1); })(5);
///     assert(r == 5*4*3*2*1);
/// 

#include <boost/fit/always.hpp>
#include <boost/fit/detail/callable_base.hpp>
#include <boost/fit/reveal.hpp>
#include <boost/fit/detail/delegate.hpp>
#include <boost/fit/detail/move.hpp>
#include <boost/fit/detail/make.hpp>
#include <boost/fit/detail/static_const_var.hpp>

#ifndef BOOST_FIT_FIX_HAS_CONSTEXPR
#define BOOST_FIT_FIX_HAS_CONSTEXPR 0
#endif

#if BOOST_FIT_FIX_HAS_CONSTEXPR
#define BOOST_FIT_FIX_CONSTEXPR constexpr
#else
#define BOOST_FIT_FIX_CONSTEXPR
#endif


namespace boost { namespace fit {

namespace detail{

template<class Derived, class F>
struct fix_adaptor_base : detail::callable_base<F>
{
    BOOST_FIT_INHERIT_CONSTRUCTOR(fix_adaptor_base, detail::callable_base<F>);

    template<class... Ts>
    BOOST_FIT_FIX_CONSTEXPR const detail::callable_base<F>& base_function(Ts&&... xs) const
    {
        return always_ref(*this)(xs...);
    }

    template<class... Ts>
    BOOST_FIT_FIX_CONSTEXPR const Derived& derived_function(Ts&&... xs) const
    {
        return static_cast<const Derived&>(always_ref(*this)(xs...));
    }

    struct fix_failure
    {
        template<class Failure>
        struct apply
        {
            template<class... Ts>
            struct of
            : Failure::template of<Derived, Ts...>
            {};
        };
    };

    struct failure
    : failure_map<fix_failure, detail::callable_base<F>>
    {};


    BOOST_FIT_RETURNS_CLASS(fix_adaptor_base);

    template<class... Ts>
    BOOST_FIT_FIX_CONSTEXPR BOOST_FIT_SFINAE_RESULT(const detail::callable_base<F>&, id_<const Derived&>, id_<Ts>...) 
    operator()(Ts&&... xs) const BOOST_FIT_SFINAE_RETURNS
    (
        BOOST_FIT_MANGLE_CAST(const detail::callable_base<F>&)(BOOST_FIT_CONST_THIS->base_function(xs...))
            (BOOST_FIT_MANGLE_CAST(const Derived&)(BOOST_FIT_CONST_THIS->derived_function(xs...)), boost::fit::forward<Ts>(xs)...)
    );
};
}

template<class F>
struct fix_adaptor : detail::fix_adaptor_base<fix_adaptor<F>, F>
{
    typedef fix_adaptor fit_rewritable1_tag;
    typedef detail::fix_adaptor_base<fix_adaptor<F>, F> base;
    BOOST_FIT_INHERIT_CONSTRUCTOR(fix_adaptor, base);
};

BOOST_FIT_DECLARE_STATIC_VAR(fix, detail::make<fix_adaptor>);

}} // namespace boost::fit

#endif
