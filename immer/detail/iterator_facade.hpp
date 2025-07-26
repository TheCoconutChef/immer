//
// immer: immutable data structures for C++
// Copyright (C) 2016, 2017, 2018 Juan Pedro Bolivar Puente
//
// This software is distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt
//

#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace immer {
namespace detail {

struct iterator_core_access
{
    template <typename T>
    static decltype(auto) dereference(T&& x)
    {
        return x.dereference();
    }

    template <typename T>
    static decltype(auto) increment(T&& x)
    {
        return x.increment();
    }

    template <typename T>
    static decltype(auto) decrement(T&& x)
    {
        return x.decrement();
    }

    template <typename T1, typename T2>
    static decltype(auto) equal(T1&& x1, T2&& x2)
    {
        return x1.equal(x2);
    }

    template <typename T, typename D>
    static decltype(auto) advance(T&& x, D d)
    {
        return x.advance(d);
    }

    template <typename T1, typename T2>
    static decltype(auto) distance_to(T1&& x1, T2&& x2)
    {
        return x1.distance_to(x2);
    }
};

template <typename DerivedT,
          typename T,
          typename ReferenceT      = T&,
          typename DifferenceTypeT = std::ptrdiff_t,
          typename PointerT        = T*>
class forward_iterator_facade
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = DifferenceTypeT;
    using pointer           = PointerT;
    using reference         = ReferenceT;

protected:
    using access_t = iterator_core_access;

    template <typename SelfT>
    const DerivedT& derived(const SelfT* self) const
    {
        static_assert(std::is_base_of<SelfT, DerivedT>::value,
                      "must pass a derived thing");
        return *static_cast<const DerivedT*>(self);
    }

    template <typename SelfT>
    DerivedT& derived(SelfT* self)
    {
        static_assert(std::is_base_of<SelfT, DerivedT>::value,
                      "must pass a derived thing");
        return *static_cast<DerivedT*>(self);
    }

public:
    ReferenceT operator*() const
    {
        return access_t::dereference(derived(this));
    }
    PointerT operator->() const
    {
        return &access_t::dereference(derived(this));
    }

    friend bool operator==(const DerivedT& a, const DerivedT& b)
    {
        return access_t::equal(a, b);
    }

    friend bool operator!=(const DerivedT& a, const DerivedT& b)
    {
        return !access_t::equal(a, b);
    }

    DerivedT& operator++()
    {
        access_t::increment(derived(this));
        return derived(this);
    }

    DerivedT operator++(int)
    {
        auto tmp = derived(this);
        access_t::increment(derived(this));
        return tmp;
    }

    DerivedT& operator+=(DifferenceTypeT n)
    {
        access_t::advance(derived(this), n);
        return derived(this);
    }

    DerivedT& operator-=(DifferenceTypeT n)
    {
        access_t::advance(derived(this), -n);
        return derived(this);
    }
};

template <typename DerivedT,
          typename T,
          typename ReferenceT      = T&,
          typename DifferenceTypeT = std::ptrdiff_t,
          typename PointerT        = T*>
class bidirectional_iterator_facade
    : public forward_iterator_facade<DerivedT,
                                     T,
                                     ReferenceT,
                                     DifferenceTypeT,
                                     PointerT>
{
    using base_t = forward_iterator_facade<DerivedT,
                                           T,
                                           ReferenceT,
                                           DifferenceTypeT,
                                           PointerT>;

public:
    using iterator_category = std::bidirectional_iterator_tag;

    DerivedT& operator--()
    {
        base_t::access_t::decrement(base_t::derived(this));
        return base_t::derived(this);
    }

    DerivedT operator--(int)
    {
        auto tmp = base_t::derived(this);
        base_t::access_t::decrement(base_t::derived(this));
        return tmp;
    }
};

template <typename DerivedT,
          typename T,
          typename ReferenceT      = T&,
          typename DifferenceTypeT = std::ptrdiff_t,
          typename PointerT        = T*>
class random_access_iterator_facade
    : public bidirectional_iterator_facade<DerivedT,
                                           T,
                                           ReferenceT,
                                           DifferenceTypeT,
                                           PointerT>
{
    using base_t = bidirectional_iterator_facade<DerivedT,
                                                 T,
                                                 ReferenceT,
                                                 DifferenceTypeT,
                                                 PointerT>;

public:
    using iterator_category = std::random_access_iterator_tag;

    ReferenceT operator[](DifferenceTypeT n) const
    {
        return *(base_t::derived(this) + n);
    }

    DerivedT operator+(DifferenceTypeT n) const
    {
        auto tmp = base_t::derived(this);
        return tmp += n;
    }

    friend DerivedT operator+(DifferenceTypeT n, const DerivedT& i)
    {
        return i + n;
    }

    DerivedT operator-(DifferenceTypeT n) const
    {
        auto tmp = base_t::derived(this);
        return tmp -= n;
    }

    friend DifferenceTypeT operator-(const DerivedT& a, const DerivedT& b)
    {
        return base_t::access_t::distance_to(b, a);
    }

    friend bool operator<(const DerivedT& a, const DerivedT& b)
    {
        return base_t::access_t::distance_to(a, b) > 0;
    }

    friend bool operator<=(const DerivedT& a, const DerivedT& b)
    {
        return base_t::access_t::distance_to(a, b) >= 0;
    }

    friend bool operator>(const DerivedT& a, const DerivedT& b)
    {
        return base_t::access_t::distance_to(a, b) < 0;
    }

    friend bool operator>=(const DerivedT& a, const DerivedT& b)
    {
        return base_t::access_t::distance_to(a, b) <= 0;
    }
};

} // namespace detail
} // namespace immer
