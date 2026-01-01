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
#include "SourceMap/Data.h"
#include "SourceMap/Entry.h"

#include <filesystem>
#include <memory>
#include <span>
#include <string>

namespace SourceMap {

/// @brief implements a search interface
/// * find the original entry for a position in the generated text
/// * list all original files involved
///
/// This class encapsulates a state and generates search indices
/// It's cheap to copy and uses an internal shared_pointer pimpl
template<typename... ExtensionTypes>
class Mapping {
public:
    using Entry = SourceMap::Entry<ExtensionTypes...>;
    using Entries = SourceMap::Entries<ExtensionTypes...>;
    using Data = SourceMap::Data<ExtensionTypes...>;
    using Extensions = SourceMap::Extensions<ExtensionTypes...>;

public:
    Mapping() = default;
    explicit Mapping(Data data);

    /// @returns raw data
    auto data() const -> Data const&;

    /// @returns sorted Entries
    auto entriesSortedToGeneratedPosition() const -> Entries const&;

    /// @returns List of all files involved in the generation
    /// List has no duplicates
    auto originalNames() const -> std::span<std::filesystem::path const>;

    /// @returns nullptr for an invalid position
    /// Pointer to the mapping entry of the generated position
    auto findEntryByGenerated(Position const&) const -> Entry const*;

private:
    class Private;
    std::shared_ptr<Private> m_private;
};

} // namespace SourceMap

#include "Mapping_impl.h"
