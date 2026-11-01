// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "SourceMap/Entry.h"
#include "SourceMap/Extensions.h"

namespace SourceMap {

/// @brief encapsulates all input data of a source map
/// * each entry marks the mapping of starting positions
/// * supports any number of extensions
template<typename... ExtensionTypes>
struct Data {
    using Entry = SourceMap::Entry<ExtensionTypes...>;
    using Entries = SourceMap::Entries<ExtensionTypes...>;
    using Extensions = SourceMap::Extensions<ExtensionTypes...>;
    using ExtensionData = typename Extensions::MapData;

    Entries entries;
    ExtensionData extensionData;

    bool addEntry(Entry const& entry);
};

template<class T>
concept IsDataTpl = requires(T t) { []<typename... ExtensionTypes>(Data<ExtensionTypes...> const&) {}(t); };

template<typename ExtensionType, IsDataTpl Data>
inline auto get(Data&& data)
    -> decltype(std::get<typename ExtensionType::MapData>(std::forward<Data>(data).extensionData)) {
    return std::get<typename ExtensionType::MapData>(std::forward<Data>(data).extensionData);
}

} // namespace SourceMap

#include "Data_impl.h"
