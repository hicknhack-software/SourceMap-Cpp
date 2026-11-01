// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "SourceMap/Extensions.h"
#include "SourceMap/FilePosition.h"
#include "SourceMap/Position.h"

#include <span>
#include <string>
#include <vector>

namespace SourceMap {

template<typename... ExtensionTypes>
struct Entry;

template<typename... ExtensionTypes>
using Entries = std::vector<Entry<ExtensionTypes...>>;
template<typename... ExtensionTypes>
using EntrySpan = std::span<Entry<ExtensionTypes...>>;

/// @brief represents the data for one mapping entry
/// * maps a position of the original text to a position of the generated text
/// * supports any number of extensions
/// this is a pure data container structure
template<typename... ExtensionTypes>
struct Entry {
    using Extensions = SourceMap::Extensions<ExtensionTypes...>;
    using ExtensionData = typename Extensions::EntryData;

    Position generated; ///< text start position in the generated file
    FilePosition original; ///< text start position in the original/source file
    std::string name; ///< name of the symbol (used for debuggers)
    ExtensionData extensionData; ///< addtional data for all extensions

    /// @returns true if the entry is useful
    constexpr auto isValid() const -> bool { return generated.isValid(); }
};

template<class T>
concept IsEntryTpl = requires(T t) { []<typename... ExtensionTypes>(Entry<ExtensionTypes...> const&) {}(t); };

template<typename ExtensionType, IsEntryTpl Entry>
constexpr auto get(Entry&& entry)
    -> decltype(std::get<typename ExtensionType::EntryData>(std::forward<Entry>(entry).extensionData)) {
    return std::get<typename ExtensionType::EntryData>(std::forward<Entry>(entry).extensionData);
}

} // namespace SourceMap
