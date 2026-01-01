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
#include "SourceMap/RevisionThree.h"

namespace SourceMap {

namespace intern {

inline auto const MAPPINGS_KEY = std::string_view{"mappings"};

void jsonStoreMappings(std::reference_wrapper<RevisionThree> json, std::string&& mappings);

void jsonDecodeMappings(
    RevisionThree const& json, std::move_only_function<void(Position&&, FilePosition&&, std::string&&)> callback);

struct MappingsEncoder {
    MappingsEncoder(std::span<std::filesystem::path const> sources, std::span<std::string const> nameList)
        : sources{sources}
        , nameList{nameList} {}

    void add(Position const& generated, FilePosition const& original, std::string const& name);

    std::string build();

public:
    std::vector<std::string> groups;
    std::span<std::filesystem::path const> sources;
    std::span<std::string const> nameList;
    int generatedLine = 1;
    int generatedColumn = 1;
    int sourceIndex = 0;
    int sourceLine = 1;
    int sourceColumn = 1;
    int nameIndex = 0;
    std::vector<std::string> segments;
};

template<typename Mapping>
auto extractEntryList(Mapping const& mapping) -> typename Mapping::Entries const& {
    return mapping.entriesSortedToGeneratedPosition();
}

template<typename Mapping>
auto extractSources(Mapping const& mapping) -> std::span<std::filesystem::path const> {
    return mapping.originalNames();
}

template<typename Mapping>
auto extractNameList(Mapping const& mapping) -> std::vector<std::string> {
    std::vector<std::string> names;
    for (auto& entry : extractEntryList(mapping)) {
        if (entry.name.empty()) continue;
        if (std::none_of(names.begin(), names.end(), [&](std::string const& v) { return v == entry.name; })) {
            names.push_back(entry.name);
        }
    }
    return names;
}

template<typename Mapping>
auto encodeMappingString(
    std::span<std::filesystem::path const> sources, std::span<std::string const> names, Mapping const& mapping)
    -> std::string {
    auto encoder = MappingsEncoder{sources, names};
    for (auto const& entry : extractEntryList(mapping)) encoder.add(entry.generated, entry.original, entry.name);
    return encoder.build();
}

} // namespace intern

template<typename Data>
auto RevisionThree::decodedMappings() const -> Data {
    typename Data::Entries entries;
    intern::jsonDecodeMappings(*this, [&](Position&& generated, FilePosition&& original, std::string&& name) {
        entries.emplace_back(std::move(generated), std::move(original), std::move(name));
    });
    auto data = Data{.entries = std::move(entries)};
    Data::Extensions::jsonDecode(std::ref(data), *this);
    return data;
}

template<typename Mapping>
void RevisionThree::encodeMappings(Mapping const& mappings) {
    auto const sources = intern::extractSources(mappings);
    storeSources(sources);
    auto const names = intern::extractNameList(mappings);
    storeNames(names);
    intern::jsonStoreMappings(std::ref(*this), intern::encodeMappingString(sources, names, mappings));
    Mapping::Extensions::jsonEncode(mappings.data(), std::ref(*this));
}

} // namespace SourceMap
