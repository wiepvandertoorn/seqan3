// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Adaptations of concepts from the standard library.
 * \author Hannes Hauswedell <hannes.hauswedell AT fu-berlin.de>
 */

#pragma once

#include <initializer_list>
#include <iterator>
#include <type_traits>

// remove if is_basic_string is not needed anymore
#include <string>

#include <seqan3/std/iterator>
#include <seqan3/std/concepts>

#include <seqan3/core/platform.hpp>

// TODO:
// * remove is_basic_string
// * remove #include <string> in this file
// once the gcc bug [1] was fixed AND we require at least a gcc version which
// contains this fix
//
// [1] https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83328
namespace seqan3::detail
{
//!\privatesection

//!\brief Returns whether `basic_string_t` is of type `std::basic_string<value_t, traits_t, allocator_t>`.
//!\attention Will be deleted once seqan3::detail::sequence_container_modified_by_const_iterator_bug is fixed.
template <typename basic_string_t>
struct is_basic_string : std::false_type
{};

//!\brief Returns whether `basic_string_t` is of type `std::basic_string<value_t, traits_t, allocator_t>`.
//!\attention Will be deleted once seqan3::detail::sequence_container_modified_by_const_iterator_bug is fixed.
template <typename value_t, typename traits_t, typename allocator_t>
struct is_basic_string<std::basic_string<value_t, traits_t, allocator_t>> : std::true_type
{};

//!\brief Shorthand of seqan3::detail::is_basic_string
//!\attention Will be deleted once seqan3::detail::sequence_container_modified_by_const_iterator_bug is fixed.
template <typename basic_string_t>
constexpr bool is_basic_string_v = is_basic_string<basic_string_t>::value;

//!\publicsection

} // seqan3::detail

namespace seqan3
{

/*!\addtogroup container
 * \{
 */
/*!\interface seqan3::container <>
 * \extends std::ranges::forward_range
 * \extends std::ranges::sized_range
 * \extends std::ranges::common_range
 * \extends seqan3::const_iterable_range
 * \brief The (most general) container concept as defined by the standard library.
 * \details
 * The container concept is modelled as in the [STL](https://en.cppreference.com/w/cpp/concept/Container), but
 * additionally requires std::swap() to be implemented.
 *
 * \attention
 * Other than one might expect, `std::forward_list` does not satisfy this concept (because it does not provide
 * `.size()`).
 */
//!\cond
template <typename type>
SEQAN3_CONCEPT container = requires (type val, type val2, type const cval, typename type::iterator it)
{
    // member types
    typename type::value_type;
    typename type::reference;
    typename type::const_reference;
/*
    typename type::iterator;
    requires std::forward_iterator<typename type::iterator>;
    // NOTE check whether iterator is const convertible
    SEQAN3_RETURN_TYPE_CONTRAINT(it, std::same_as, typename type::const_iterator);

    typename type::const_iterator;
    requires std::forward_iterator<typename type::const_iterator>;

    typename type::difference_type;
    typename type::size_type;
    requires std::is_same_v<
        typename type::difference_type,
        typename std::iterator_traits<typename type::iterator>::difference_type
    >;
    requires std::is_same_v<
        typename std::iterator_traits<typename type::iterator>::difference_type,
        typename std::iterator_traits<typename type::const_iterator>::difference_type
    >;
*/
    // methods and operator
    SEQAN3_RETURN_TYPE_CONTRAINT(type{}, std::same_as, type); // default constructor
    SEQAN3_RETURN_TYPE_CONTRAINT(type{type{}}, std::same_as, type); // copy/move constructor
    SEQAN3_RETURN_TYPE_CONTRAINT(val = val2, std::same_as, type &); // assignment
    { (&val)->~type() }; // destructor

    SEQAN3_RETURN_TYPE_CONTRAINT(val.begin(), std::same_as, typename type::iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.end(), std::same_as, typename type::iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(cval.begin(), std::same_as, typename type::const_iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(cval.end(), std::same_as, typename type::const_iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.cbegin(), std::same_as, typename type::const_iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.cend(), std::same_as, typename type::const_iterator);

    requires !std::equality_comparable<typename type::value_type> || std::equality_comparable<type>;

    SEQAN3_RETURN_TYPE_CONTRAINT(val.swap(val2), std::same_as, void);
    SEQAN3_RETURN_TYPE_CONTRAINT(swap(val, val2), std::same_as, void);
    SEQAN3_RETURN_TYPE_CONTRAINT(std::swap(val, val2), std::same_as, void);

    SEQAN3_RETURN_TYPE_CONTRAINT(val.size(), std::same_as, typename type::size_type);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.max_size(), std::same_as, typename type::size_type);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.empty(), std::same_as, bool);
};
//!\endcond

/*!\interface seqan3::sequence_container <>
 * \extends seqan3::container
 * \brief A more refined container concept than seqan3::container.
 *
 * Includes constraints on constructors, `assign()`, `.insert()`, `.erase()`, `.push_back()`, `.pop_back`, `.clear()`,
 * `.size()`, `front()` and `.back()` member functions with corresponding signatures. Models the subset of the
 * [STL SequenceConcept](https://en.cppreference.com/w/cpp/concept/SequenceConcept) that is supported
 * by `std::list`, `std::vector`, `std::deque`, `std::basic_string`.
 *
 * \attention
 * `std::array` and `std::forward_list` do not satisfy this concept.
 */
//!\cond
template <typename type>
SEQAN3_CONCEPT sequence_container = requires (type val, type val2, type const cval)
{
    requires container<type>;

    // construction
    { type{typename type::size_type{}, typename type::value_type{}} };
    { type{val2.begin(), val2.end()}                                }; // NOTE that this could be any input iterator:
    { type{std::initializer_list<typename type::value_type>{}}      };
    SEQAN3_RETURN_TYPE_CONTRAINT(val = std::initializer_list<typename type::value_type>{}, std::same_as, type &);

    // assignment NOTE return type is type & for std::string and void for other stl containers:
    { val.assign(val2.begin(), val2.end())                                };
    { val.assign(std::initializer_list<typename type::value_type>{})      };
    { val.assign(typename type::size_type{}, typename type::value_type{}) };

    // modify container
    // TODO: how do you model this?
    // SEQAN3_RETURN_TYPE_CONTRAINT(val.emplace(typename type::const_iterator{}, ?),
    //                              std::same_as, typename type::iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.insert(val.cbegin(), val2.front()), std::same_as, typename type::iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.insert(val.cbegin(), typename type::value_type{}),
                                 std::same_as, typename type::iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.insert(val.cbegin(), typename type::size_type{}, typename type::value_type{}),
                                 std::same_as, typename type::iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.insert(val.cbegin(), val2.begin(), val2.end()),
                                 std::same_as, typename type::iterator);
    requires detail::is_basic_string_v<type> || requires(type val)
    {
        // TODO this function is not defined on strings (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83328)
        SEQAN3_RETURN_TYPE_CONTRAINT(val.insert(val.cbegin(), std::initializer_list<typename type::value_type>{}),
                                     std::same_as, typename type::iterator);
    };

    SEQAN3_RETURN_TYPE_CONTRAINT(val.erase(val.cbegin()), std::same_as, typename type::iterator);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.erase(val.cbegin(), val.cend()), std::same_as, typename type::iterator);

    SEQAN3_RETURN_TYPE_CONTRAINT(val.push_back(val.front()), std::same_as, void);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.push_back(typename type::value_type{}), std::same_as, void);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.pop_back(), std::same_as, void);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.clear(), std::same_as, void);

    // access container
    SEQAN3_RETURN_TYPE_CONTRAINT(val.front(), std::same_as, typename type::reference);
    SEQAN3_RETURN_TYPE_CONTRAINT(cval.front(), std::same_as, typename type::const_reference);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.back(), std::same_as, typename type::reference);
    SEQAN3_RETURN_TYPE_CONTRAINT(cval.back(), std::same_as, typename type::const_reference);
};
//!\endcond

/*!\interface seqan3::random_access_container <>
 * \extends seqan3::sequence_container
 * \extends std::ranges::random_access_range
 * \brief A more refined container concept than seqan3::sequence_container.
 *
 * Adds requirements for `.at()`, `.resize()` and the subscript operator `[]`. Models the subset of the
 * [STL SequenceConcept](httpa://en.cppreference.com/w/cpp/concept/SequenceConcept) that is supported
 * by `std::vector`, `std::deque` and `std::basic_string`.
 *
 * \attention
 * `std::array`, `std::forward_list` and `std::list` do not satisfy this concept.
 *
 * \sa
 */
//!\cond
template <typename type>
SEQAN3_CONCEPT random_access_container = requires (type val)
{
    requires sequence_container<type>;

    // access container
    SEQAN3_RETURN_TYPE_CONTRAINT(val[0], std::same_as, typename type::reference);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.at(0), std::same_as, typename type::reference);

    // modify container
    SEQAN3_RETURN_TYPE_CONTRAINT(val.resize(0), std::same_as, void);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.resize(0, typename type::value_type{}), std::same_as, void);
};
//!\endcond

/*!\interface seqan3::reservible_container <>
 * \extends seqan3::random_access_container
 * \brief A more refined container concept than seqan3::random_access_container.
 *
 * Adds requirements for `.reserve()`, `.capacity()` and `.shrink_to_fit()`.
 * Satisfied by `std::vector` and `std::basic_string`.
 *
 * \attention
 * `std::array`, `std::forward_list`, `std::list` and `std::deque` do not satisfy this concept.
 */
//!\cond
template <typename type>
SEQAN3_CONCEPT reservible_container = requires (type val)
{
    requires random_access_container<type>;

    SEQAN3_RETURN_TYPE_CONTRAINT(val.capacity(), std::same_as, typename type::size_type);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.reserve(0), std::same_as, void);
    SEQAN3_RETURN_TYPE_CONTRAINT(val.shrink_to_fit(), std::same_as, void);
};
//!\endcond

//!\}

} // namespace seqan3
