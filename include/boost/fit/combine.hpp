/*=============================================================================
    Copyright (c) 2015 Paul Fultz II
    combine.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_COMBINE_H
#define BOOST_FIT_GUARD_COMBINE_H

/// combine
/// =======
/// 
/// Description
/// -----------
/// 
/// The `combine` function adaptor combines several functions together with
/// their arguments. It essentially zips each function with an argument before
/// calling the main function.
/// 
/// Synopsis
/// --------
/// 
///     template<class F, class... Gs>
///     constexpr combine_adaptor<F, Gs...> combine(F f, Gs... gs);
/// 
/// Semantics
/// ---------
/// 
///     assert(combine(f, gs...)(xs...) == f(gs(xs)...));
/// 
/// Requirements
/// ------------
/// 
/// F and Gs must be:
/// 
/// * [Callable](concepts.md#callable)
/// * MoveConstructible
/// 
/// Example
/// -------
/// 
///     auto f = boost::fit::combine(
///         boost::fit::construct<std::tuple>(),
///         boost::fit::capture(1)(boost::fit::construct<std::pair>()),
///         boost::fit::capture(2)(boost::fit::construct<std::pair>()));
///     assert(f(2, 4) == std::make_tuple(std::make_pair(1, 2), std::make_pair(2, 4)));
/// 

#include <boost/fit/pack.hpp>
#include <boost/fit/always.hpp>
#include <boost/fit/detail/callable_base.hpp>
#include <boost/fit/detail/make.hpp>

namespace boost { namespace fit { namespace detail {

template<class S, class F, class... Gs>
struct combine_adaptor_base;

template<std::size_t... Ns, class F, class... Gs>
struct combine_adaptor_base<seq<Ns...>, F, Gs...>
: F, pack_base<seq<Ns...>, Gs...>
{
    typedef pack_base<seq<Ns...>, Gs...> base_type;

    BOOST_FIT_INHERIT_DEFAULT(combine_adaptor_base, base_type, F)

    template<class X, class... Xs, 
        BOOST_FIT_ENABLE_IF_CONSTRUCTIBLE(F, X),
        BOOST_FIT_ENABLE_IF_CONSTRUCTIBLE(base_type, Xs...)>
    constexpr combine_adaptor_base(X&& x, Xs&&... xs) 
    : F(boost::fit::forward<X>(x)), base_type(boost::fit::forward<Xs>(xs)...)
    {}

    template<class... Ts>
    constexpr const F& base_function(Ts&&... xs) const
    {
        return always_ref(*this)(xs...);
    }

    BOOST_FIT_RETURNS_CLASS(combine_adaptor_base);

// Result needs to be calculated in a separate class to avoid confusing the
// compiler on MSVC
#if BOOST_FIT_NO_EXPRESSION_SFINAE || BOOST_FIT_HAS_MANUAL_DEDUCTION
    template<class... Ts>
    struct combine_result
    : result_of<const F&,  result_of<const Gs&, id_<Ts>>...>
    {};
#endif

    template<class... Ts>
#if BOOST_FIT_NO_EXPRESSION_SFINAE || BOOST_FIT_HAS_MANUAL_DEDUCTION
    constexpr typename combine_result<Ts...>::type
#else
    constexpr auto
#endif
    operator()(Ts&&... xs) const BOOST_FIT_SFINAE_MANUAL_RETURNS
    (
        (BOOST_FIT_MANGLE_CAST(const F&)(BOOST_FIT_CONST_THIS->base_function(xs...)))
            (alias_value<pack_tag<seq<Ns>, Gs...>, Gs>(*BOOST_FIT_CONST_THIS, xs)(boost::fit::forward<Ts>(xs))...)
    );
};

}

template<class F, class... Gs>
struct combine_adaptor
: detail::combine_adaptor_base<typename detail::gens<sizeof...(Gs)>::type, detail::callable_base<F>, detail::callable_base<Gs>...>
{
    typedef detail::combine_adaptor_base<typename detail::gens<sizeof...(Gs)>::type, detail::callable_base<F>, detail::callable_base<Gs>...> base_type;
    BOOST_FIT_INHERIT_CONSTRUCTOR(combine_adaptor, base_type)
};

BOOST_FIT_DECLARE_STATIC_VAR(combine, detail::make<combine_adaptor>);

}} // namespace boost::fit

#endif
