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
#include "SourceMap/Entry.h"
#include "SourceMap/Extension/Interpolation.h"

#include <vector>

namespace SourceMap {

template<typename... ExtensionTypes>
auto getOriginalPositionFromGenerated(Mapping<ExtensionTypes...> const& mapping, Position const& position)
    -> FilePosition {
    using Entry = typename Mapping<ExtensionTypes...>::Entry;

    auto const* entry = mapping.findEntryByGenerated(position);
    if (entry == nullptr) {
        return {};
    }
    if (get<Extension::Interpolation>(*entry) == Interpolation::None) {
        return entry->original;
    }
    return {
        .name = entry->original.name,
        .position =
            Position{
                .line = (position.line - entry->generated.line) + entry->original.position.line,
                .column = (position.line == entry->generated.line)
                    ? (position.column - entry->generated.column) + entry->original.position.column
                    : position.column,
            },
    };
}

namespace Extension {
namespace intern {

/// Pair of Interpolation state and it's repetition count
using InterpolationList = std::vector<SourceMap::Interpolation>;
using GeneratedLineInterpolationList = std::vector<std::tuple<int, SourceMap::Interpolation>>;

InterpolationList jsonDecodeInterpolationList(RevisionThree const& json);
void jsonStoreInterpolations(
    std::reference_wrapper<RevisionThree> json, GeneratedLineInterpolationList const& interpolations);

template<typename Data>
auto extractInterpolationList(Data const& data) -> GeneratedLineInterpolationList {
    using Interpolation = SourceMap::Extension::Interpolation;
    auto result = GeneratedLineInterpolationList{};
    for (auto const& entry : data.entries) {
        auto const entryInterpolation = SourceMap::get<Interpolation>(entry);
        result.emplace_back(entry.generated.line, entryInterpolation);
    }
    return result;
}

template<typename Data>
void injectInterpolationList(std::reference_wrapper<Data> data, intern::InterpolationList&& list) {
    auto begin = list.begin();
    auto const end = list.end();
    for (auto& entry : data.get().entries) {
        auto entryInterpolation = SourceMap::Interpolation{};
        if (begin != end) {
            entryInterpolation = *begin++;
        }
        SourceMap::get<SourceMap::Extension::Interpolation>(entry) = entryInterpolation;
    }
}

} // namespace intern

template<typename Data>
void Interpolation::jsonEncode(Data const& data, std::reference_wrapper<RevisionThree> json) {
    intern::jsonStoreInterpolations(json, intern::extractInterpolationList(data));
}

template<typename Data>
auto Interpolation::jsonDecode(std::reference_wrapper<Data> data, RevisionThree const& json) -> bool {
    intern::injectInterpolationList(data, intern::jsonDecodeInterpolationList(json));
    return true;
}

} // namespace Extension
} // namespace SourceMap
