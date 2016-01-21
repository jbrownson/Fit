/*=============================================================================
    Copyright (c) 2014 Paul Fultz II
    indirect.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_FUNCTION_INDIRECT_H
#define BOOST_FIT_GUARD_FUNCTION_INDIRECT_H

/// indirect
/// ========
/// 
/// Description
/// -----------
/// 
/// The `indirect` function adaptor dereferences the object before calling it.
/// 
/// Synopsis
/// --------
/// 
///     template<class F>
///     constexpr indirect_adaptor<F> indirect(F f);
/// 
/// Semantics
/// ---------
/// 
///     assert(indirect(f)(xs...) == (*f)(xs...));
/// 
/// Requirements
/// ------------
/// 
/// F must be:
/// 
/// * [FunctionObject](concepts.md#functionobject)
/// * MoveConstructible
/// * Dereferenceable
/// 
/// Example
/// -------
/// 
///     struct sum
///     {
///         template<class T, class U>
///         T operator()(T x, U y) const
///         {
///             return x+y;
///         }
///     };
/// 
///     int r = indirect(std::make_unique<sum>())(3,2);
///     assert(r == 5);
/// 

#include <boost/fit/detail/delegate.hpp>
#include <boost/fit/detail/result_of.hpp>
#include <boost/fit/reveal.hpp>
#include <boost/fit/always.hpp>
#include <boost/fit/detail/move.hpp>
#include <boost/fit/detail/make.hpp>
#include <boost/fit/detail/static_const_var.hpp>

namespace boost { namespace fit {
// TODO: Support non-classes as well
template<class F>
struct indirect_adaptor : F
{
    typedef indirect_adaptor fit_rewritable1_tag;
    BOOST_FIT_INHERIT_CONSTRUCTOR(indirect_adaptor, F);

    template<class... Ts>
    constexpr const F& base_function(Ts&&... xs) const
    {
        return always_ref(*this)(xs...);
    }

    struct failure
    : failure_for<decltype(*std::declval<F>())>
    {};

    BOOST_FIT_RETURNS_CLASS(indirect_adaptor);

    template<class... Ts>
    constexpr BOOST_FIT_SFINAE_RESULT(decltype(*std::declval<F>()), id_<Ts>...) 
    operator()(Ts&&... xs) const BOOST_FIT_SFINAE_RETURNS
    (
        (*BOOST_FIT_MANGLE_CAST(const F&)(BOOST_FIT_CONST_THIS->base_function(xs...)))(boost::fit::forward<Ts>(xs)...)
    );
};

BOOST_FIT_DECLARE_STATIC_VAR(indirect, detail::make<indirect_adaptor>);

}} // namespace boost::fit

#endif
