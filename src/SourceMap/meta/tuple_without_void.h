// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include <tuple>

namespace SourceMap::meta {

template<typename tuple, typename... Types>
struct tuple_without_void;

template<typename... Types>
struct tuple_without_void<std::tuple<Types...>> {
    using type = std::tuple<Types...>;
};

template<typename... Types, typename... Tail>
struct tuple_without_void<std::tuple<Types...>, void, Tail...> {
    using type = typename tuple_without_void<std::tuple<Types...>, Tail...>::type;
};

template<typename... Types, typename T, typename... Tail>
struct tuple_without_void<std::tuple<Types...>, T, Tail...> {
    using type = typename tuple_without_void<std::tuple<Types..., T>, Tail...>::type;
};

/// @brief meta function that constructs a std::tuple from a parameter pack skipping void types
template<typename... Types>
using tuple_without_void_t = typename tuple_without_void<std::tuple<>, Types...>::type;

} // namespace SourceMap::meta
