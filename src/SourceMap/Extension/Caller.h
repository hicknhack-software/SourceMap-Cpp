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
#include "SourceMap/Extension/Base.h"
#include "SourceMap/FilePosition.h"

#include <vector>

namespace SourceMap {

template<typename... ExtensionTypes>
struct Entry;

template<typename... ExtensionTypes>
struct Data;

template<typename... ExtensionTypes>
class Mapping;

struct Caller;
using CallerList = std::vector<Caller>;

using CallerStack = std::vector<FilePosition>;

constexpr auto InvalidCallerIndex = -1;

/// @brief encapsulates the index to a caller
/// this ist an basic type with meaning
struct CallerIndex {
    int value = InvalidCallerIndex;

    bool operator==(CallerIndex const&) const = default;
};

/// @brief represents an entry in a call stack
/// Any instance with an invalid origin position is considered invalid
/// this is a pure data container structure
struct Caller {
    FilePosition original; ///< source position
    CallerIndex parentIndex; ///< index of the parent caller (-1 if none)

    inline auto isValid() const -> bool { return original.isValid(); }

    bool operator==(Caller const&) const = default;
};

/// @brief builds a call stack of a given entry
/// @param data SourceMap::Data where entry is part of
/// @param entry Pointer to an entry of data
/// @return List of Callers that are involved in building this entry
template<typename... ExtensionTypes>
auto buildCallerStack(Data<ExtensionTypes...> const& data, Entry<ExtensionTypes...> const* entry) -> CallerStack;

/// @brief builds a call stack of a given entry
/// @param mapping SourceMap::Mapping where entry is part of
/// @param entry Pointer to an entry of data
/// @return List of Callers that are involved in building this entry
template<typename... ExtensionTypes>
auto buildCallerStack(Mapping<ExtensionTypes...> const& mapping, Entry<ExtensionTypes...> const* entry) -> CallerStack;

namespace Extension {

struct Caller : Base {
    using EntryData = CallerIndex; ///< one index per entry
    using MapData = CallerList; ///< list of all callers

    /// @brief Stores the caller data as json entries
    /// "x_de_hicknhack_software_callstack" contains two arrays:
    ///   1. callers
    ///   2. indices
    ///
    /// "callers" encodes the array of all callers as a string
    /// * Each caller entry is seperated by ';'
    /// * Segments in a line are separated by ','
    /// * Each caller entry is encoded of 3 or 4 Base64 VLQ encoded values
    ///   1. source file index
    ///   2. source line
    ///   3. source column
    ///   4. parent caller index (missing if no caller)
    ///
    /// "indices" encodes the caller index for each entry
    /// * Each index is separated by ';'
    /// * Segments in a line are separated by ','
    /// * Base64 VLQ encoded value of the index
    template<typename Data>
    static void jsonEncode(Data const&, std::reference_wrapper<RevisionThree>);

    /// @brief Decodes caller data from json entry
    /// If it's missing no callers are loaded (no error is raised)
    template<typename Data>
    static auto jsonDecode(std::reference_wrapper<Data>, RevisionThree const&) -> bool;

    /// @brief reports files referenced in CallerList
    template<typename Data, typename Callback>
    static void collectFileNames(Data const&, Callback&&);
};

} // namespace Extension
} // namespace SourceMap

#include "Caller_impl.h"
