/*=============================================================================
    Copyright (c) 2015 Paul Fultz II
    unpack.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FIT_GUARD_UNPACK_H
#define BOOST_FIT_GUARD_UNPACK_H

/// unpack
/// ======
/// 
/// Description
/// -----------
/// 
/// The `unpack` function adaptor takes a sequence and uses the elements of
/// the sequence for the arguments to the function. Multiple sequences can be
/// passed to the function. All elements from each sequence will be passed
/// into the function. 
/// 
/// 
/// Synopsis
/// --------
/// 
///     template<class F>
///     unpack_adaptor<F> unpack(F f);
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
///     struct sum
///     {
///         template<class T, class U>
///         T operator()(T x, U y) const
///         {
///             return x+y;
///         }
///     };
/// 
///     int r = unpack(sum())(std::make_tuple(3,2));
///     assert(r == 5);
/// 
/// 
/// is_unpackable
/// =============
/// 
/// This is a trait that can be used to detect whethet the type can be called
/// with `unpack`.
/// 
/// Synopsis
/// --------
/// 
///     template<class T>
///     struct is_unpackable;
/// 
/// Example
/// -------
/// 
///     static_assert(boost::fit::is_unpackable<std::tuple<int>>::value, "Failed");
/// 
/// unpack_sequence
/// ===============
/// 
/// How to unpack a sequence can be defined by specializing `unpack_sequence`.
/// By default, `std::tuple` can be used with unpack.
/// 
/// Synopsis
/// --------
/// 
///     template<class Sequence, class=void>
///     struct unpack_sequence;
/// 
/// Example
/// -------
/// 
///     template<class... Ts>
///     struct unpack_sequence<my_sequence<Ts...>>
///     {
///         template<class F, class Sequence>
///         constexpr static auto apply(F&& f, Sequence&& s) BOOST_FIT_RETURNS
///         (
///             s(std::forward<F>(f))
///         );
///     };
/// 

#include <boost/fit/returns.hpp>
#include <tuple>
#include <boost/fit/detail/seq.hpp>
#include <boost/fit/capture.hpp>
#include <boost/fit/always.hpp>
#include <boost/fit/reveal.hpp>
#include <boost/fit/detail/and.hpp>
#include <boost/fit/detail/delegate.hpp>
#include <boost/fit/detail/holder.hpp>
#include <boost/fit/detail/move.hpp>
#include <boost/fit/detail/make.hpp>
#include <boost/fit/detail/static_const_var.hpp>

namespace boost { namespace fit {

template<class Sequence, class=void>
struct unpack_sequence
{
    typedef void not_unpackable;
};


namespace detail {
template<class Sequence, class=void>
struct is_unpackable_impl
: std::true_type
{};

template<class Sequence>
struct is_unpackable_impl<Sequence, typename detail::holder<
    typename unpack_sequence<Sequence>::not_unpackable
>::type>
: std::false_type
{};

template<class F, class Sequence>
constexpr auto unpack_impl(F&& f, Sequence&& s) BOOST_FIT_RETURNS
(
    boost::fit::unpack_sequence<typename std::remove_cv<typename std::remove_reference<Sequence>::type>::type>::
            apply(boost::fit::forward<F>(f), boost::fit::forward<Sequence>(s))
);

struct private_unpack_type {};

template<class Sequence>
constexpr int unpack_check()
{
    static_assert(
        std::is_same<
            private_unpack_type, 
            decltype(unpack_impl(always(private_unpack_type()), std::declval<Sequence>()))
        >::value, 
        "Unpack is invalid for this sequence. The function used to unpack this sequence does not invoke the function."
    );
    return 0;
}

template<class F, class Sequence, int=(unpack_check<Sequence>())>
constexpr auto unpack_simple(F&& f, Sequence&& s) BOOST_FIT_RETURNS
(
    unpack_impl(boost::fit::forward<F>(f), boost::fit::forward<Sequence>(s))
)

template<class F, class... Sequences>
constexpr auto unpack_join(F&& f, Sequences&&... s) BOOST_FIT_RETURNS
(
    boost::fit::pack_join(unpack_simple(boost::fit::pack_forward, boost::fit::forward<Sequences>(s))...)(boost::fit::forward<F>(f))
);

}

template<class Sequence>
struct is_unpackable
: detail::is_unpackable_impl<
    typename std::remove_cv<typename std::remove_reference<Sequence>::type>::type
>
{};

template<class F>
struct unpack_adaptor : detail::callable_base<F>
{
    typedef unpack_adaptor fit_rewritable1_tag;
    BOOST_FIT_INHERIT_CONSTRUCTOR(unpack_adaptor, detail::callable_base<F>);

    template<class... Ts>
    constexpr const detail::callable_base<F>& base_function(Ts&&... xs) const
    {
        return always_ref(*this)(xs...);
    }

    struct unpack_failure
    {
        template<class Failure>
        struct apply
        {
            struct deducer
            {
                template<class... Ts>
                typename Failure::template of<Ts...> operator()(Ts&&...) const;
            };

            template<class T, class=typename std::enable_if<(
                is_unpackable<T>::value
            )>::type>
            static auto deduce(T&& x)
            BOOST_FIT_RETURNS
            (
                detail::unpack_simple(deducer(), boost::fit::forward<T>(x))
            );

            template<class T, class... Ts, class=typename std::enable_if<(detail::and_<
                is_unpackable<T>, is_unpackable<Ts>...
            >::value)>::type>
            static auto deduce(T&& x, Ts&&... xs) BOOST_FIT_RETURNS
            (
                detail::unpack_join(deducer(), boost::fit::forward<T>(x), boost::fit::forward<Ts>(xs)...)
            );

            template<class... Ts>
            struct of
#if defined(__GNUC__) && !defined (__clang__) && __GNUC__ == 4 && __GNUC_MINOR__ < 7
            : std::enable_if<true, decltype(apply::deduce(std::declval<Ts>()...))>::type
#else
            : decltype(apply::deduce(std::declval<Ts>()...))
#endif
            {};
        };
    };

    struct failure
    : failure_map<unpack_failure, detail::callable_base<F>>
    {};

    BOOST_FIT_RETURNS_CLASS(unpack_adaptor);
    template<class T, class=typename std::enable_if<(
        is_unpackable<T>::value
    )>::type>
    constexpr auto operator()(T&& x) const
    BOOST_FIT_RETURNS
    (
        detail::unpack_simple(BOOST_FIT_MANGLE_CAST(const detail::callable_base<F>&)(BOOST_FIT_CONST_THIS->base_function(x)), boost::fit::forward<T>(x))
    );

    template<class T, class... Ts, class=typename std::enable_if<(detail::and_<
        is_unpackable<T>, is_unpackable<Ts>...
    >::value)>::type>
    constexpr auto operator()(T&& x, Ts&&... xs) const BOOST_FIT_RETURNS
    (
        detail::unpack_join(BOOST_FIT_MANGLE_CAST(const detail::callable_base<F>&)(BOOST_FIT_CONST_THIS->base_function(x)), boost::fit::forward<T>(x), boost::fit::forward<Ts>(xs)...)
    );
};

BOOST_FIT_DECLARE_STATIC_VAR(unpack, detail::make<unpack_adaptor>);

namespace detail {

template<class Sequence>
constexpr typename gens<std::tuple_size<Sequence>::value>::type 
make_tuple_gens(const Sequence&)
{
    return {};
}

template<class F, class... Ts, std::size_t ...N>
constexpr auto unpack_tuple(F&& f, std::tuple<Ts...> && t, seq<N...>) BOOST_FIT_RETURNS
(
    f(boost::fit::forward<Ts>(std::get<N>(t))...)
);

template<class F, class... Ts, std::size_t ...N>
constexpr auto unpack_tuple(F&& f, std::tuple<Ts...> & t, seq<N...>) BOOST_FIT_RETURNS
(
    f(boost::fit::forward<Ts>(std::get<N>(t))...)
);

template<class F, class... Ts, std::size_t ...N>
constexpr auto unpack_tuple(F&& f, const std::tuple<Ts...> & t, seq<N...>) BOOST_FIT_RETURNS
(
    f(boost::fit::forward<Ts>(std::get<N>(t))...)
);

}

template<class... Ts>
struct unpack_sequence<std::tuple<Ts...>>
{
    template<class F, class S>
    constexpr static auto apply(F&& f, S&& t) BOOST_FIT_RETURNS
    (
        detail::unpack_tuple(boost::fit::forward<F>(f), t, detail::make_tuple_gens(t))
    );
};

template<class T, class... Ts>
struct unpack_sequence<detail::pack_base<T, Ts...>>
{
    template<class F, class P>
    constexpr static auto apply(F&& f, P&& p) BOOST_FIT_RETURNS
    (
        boost::fit::detail::unpack_pack_base(boost::fit::forward<F>(f), boost::fit::forward<P>(p))
    );
};

}} // namespace boost::fit

#endif
