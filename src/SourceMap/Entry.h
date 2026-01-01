/* SourceMap-Cpp
 * (C) Copyright 2025 HicknHack Software GmbH
 *
 * The original code can be found at:
 *     https://github.com/hicknhack-software/SourceMap-Cpp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
