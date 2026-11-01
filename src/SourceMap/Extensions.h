// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "SourceMap/Extension/Base.h"
#include "SourceMap/meta/tuple_without_void.h"

#include <tuple>

namespace SourceMap {

template<typename... ExtensionTypes>
struct Entry;

template<typename... ExtensionTypes>
struct Data;

template<typename... ExtensionTypes>
class Mapping;

class RevisionThree;

/// @brief meta collection of extensions
/// This structure helps to manage the pack of extensions
/// This structure contains no data
template<typename... ExtensionTypes>
struct Extensions {
    using EntryData = meta::tuple_without_void_t<typename ExtensionTypes::EntryData...>;
    using MapData = meta::tuple_without_void_t<typename ExtensionTypes::MapData...>;

    using Entry = SourceMap::Entry<ExtensionTypes...>;
    using Data = SourceMap::Data<ExtensionTypes...>;
    using Mapping = SourceMap::Mapping<ExtensionTypes...>;
    static_assert((true && ... && Extension::ValidExtensionFor<ExtensionTypes, Data>), "All extensions must be valid");

    /// calls jsonEncode for every extension
    static void jsonEncode(Data const& data, std::reference_wrapper<RevisionThree> jsonObject) {
        static_cast<void>((ExtensionTypes::template jsonEncode<Data>(data, jsonObject), ...));
    }

    /// calls jsonDecode for every extension
    static auto jsonDecode(std::reference_wrapper<Data> data, RevisionThree const& jsonObject) -> bool {
        return (true && ... && ExtensionTypes::template jsonDecode<Data>(data, jsonObject));
    }

    template<typename Callback>
    static void collectFileNames(Data const& data, Callback&& cb) {
        static_cast<void>((ExtensionTypes::template collectFileNames<Data, Callback>(data, cb), ...));
    }
};

} // namespace SourceMap
