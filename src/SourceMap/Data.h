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
