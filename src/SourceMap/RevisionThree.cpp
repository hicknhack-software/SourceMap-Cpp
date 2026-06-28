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
#include "RevisionThree.h"
#include "SourceMap/intern/Base64VLQ.h"

#include <iostream>
#include <sstream>

namespace SourceMap {

namespace {

using namespace std::string_view_literals;

auto const VERSION_KEY = "version"sv;
auto const FILE_KEY = "file"sv;
auto const SOURCE_ROOT_KEY = "sourceRoot"sv;
auto const SOURCES_KEY = "sources"sv;
auto const SOURCES_CONTENT_KEY = "sourcesContent"sv;
auto const NAMES_KEY = "names"sv;

auto const GROUP_DELIMITER = ';';
auto const SECTION_DELIMIER = ',';

auto const PROTECTION_SEQUENCE = ")]}"sv;

} // namespace

namespace intern {

void jsonStoreMappings(std::reference_wrapper<RevisionThree> json, std::string&& mappings) {
    json.get().json()[MAPPINGS_KEY] = mappings;
}

void jsonDecodeMappings(
    RevisionThree const& json,
    std::move_only_function<void(Position&& generated, FilePosition&& original, std::string&& name)> callback) {
    auto const sourceNameAt = [sources = json.sources()](int i) -> std::filesystem::path {
        return (i >= 0 && static_cast<size_t>(i) < sources.size()) ? sources[static_cast<size_t>(i)] : std::filesystem::path{};
    };
    auto const entryNameAt = [nameList = json.names()](int i) -> std::string {
        return (i >= 0 && static_cast<size_t>(i) < nameList.size()) ? nameList[static_cast<size_t>(i)] : std::string{};
    };

    auto const encodedMappings = json.json().value(MAPPINGS_KEY, std::string{});
    auto begin = encodedMappings.begin();
    auto const end = encodedMappings.end();
    auto const load = [&begin, end](int& reference, int value) {
        if (Base64VLQ::decode(begin, end, std::ref(value))) reference = (value += reference);
        return value;
    };

    auto generated = Position{.line = 1, .column = 1};
    auto sourceIndex = 0;
    auto sourcePosition = Position{.line = 1, .column = 1};
    auto nameIndex = 0;
    while (begin != end) {
        auto ch = *begin;
        if (GROUP_DELIMITER == ch) {
            generated.line++;
            generated.column = 1;
            begin++;
            continue; // line break
        }
        if (SECTION_DELIMIER == ch) {
            begin++;
            continue; // section
        }
        auto const entryGeneratedColumn = load(generated.column, 0);
        auto const entrySourceIndex = load(sourceIndex, -1);
        auto const entrySourceLine = load(sourcePosition.line, 0);
        auto const entrySourceColumn = load(sourcePosition.column, 0);
        auto const entryNameIndex = load(nameIndex, -1);

        callback(
            Position{.line = generated.line, .column = entryGeneratedColumn},
            FilePosition{
                .name = sourceNameAt(entrySourceIndex),
                .position = Position{.line = entrySourceLine, .column = entrySourceColumn},
            },
            entryNameAt(entryNameIndex));
    }
}

void MappingsEncoder::add(Position const& generated, FilePosition const& original, std::string const& name) {
    while (generatedLine != generated.line) {
        if (!segments.empty()) {
            std::stringstream ss;
            for (size_t i = 0; i < segments.size(); ++i) {
                if (i > 0) ss << SECTION_DELIMIER;
                ss << segments[i];
            }
            groups.push_back(ss.str());
        }
        else {
            groups.push_back("");
        }
        segments.clear();
        generatedLine++;
        generatedColumn = 1;
    }
    // TODO: avoid duplicate entries
    std::string fields;
    auto store = [&](int current, int& previous) {
        Base64VLQ::encode(std::ref(fields), current - previous);
        previous = current;
    };

    store(generated.column, generatedColumn);
    if (!original.name.empty()) {
        auto it = std::find(sources.begin(), sources.end(), original.name);
        auto const entrySourceIndex = (it != sources.end()) ? std::distance(sources.begin(), it) : -1;

        store(entrySourceIndex, sourceIndex);
        store(original.position.line, sourceLine);
        store(original.position.column, sourceColumn);

        if (!name.empty()) {
            if (auto nameIt = std::find(nameList.begin(), nameList.end(), name); nameIt != nameList.end()) {
                auto const entryNameIndex = std::distance(nameList.begin(), nameIt);
                store(entryNameIndex, nameIndex);
            }
        }
    }
    segments.push_back(fields);
}

auto MappingsEncoder::build() -> std::string {
    if (!segments.empty()) {
        auto ss = std::stringstream{};
        for (size_t i = 0; i < segments.size(); ++i) {
            if (i > 0) ss << SECTION_DELIMIER;
            ss << segments[i];
        }
        groups.push_back(ss.str());
    }
    auto result = std::stringstream{};
    for (size_t i = 0; i < groups.size(); ++i) {
        if (i > 0) result << GROUP_DELIMITER;
        result << groups[i];
    }
    return result.str();
}

} // namespace intern

RevisionThree::RevisionThree() { m_json[VERSION_KEY] = 3; }

RevisionThree::RevisionThree(Json source)
    : m_json(std::move(source)) {}

auto RevisionThree::version() const -> int { return m_json.value(VERSION_KEY, 0); }

auto RevisionThree::file() const -> std::filesystem::path { return m_json.value(FILE_KEY, std::string{}); }

void RevisionThree::storeFile(std::filesystem::path const& file) { m_json[FILE_KEY] = file.string(); }

auto RevisionThree::sourceRoot() const -> std::filesystem::path { return m_json.value(SOURCE_ROOT_KEY, std::string{}); }

void RevisionThree::storeSourceRoot(std::filesystem::path const& sourceRoot) {
    m_json[SOURCE_ROOT_KEY] = sourceRoot.string();
}

auto RevisionThree::sources() const -> std::vector<std::filesystem::path> {
    auto const sourceStrings = m_json.value(SOURCES_KEY, std::vector<std::string>{});
    return std::vector<std::filesystem::path>{sourceStrings.begin(), sourceStrings.end()};
}

void RevisionThree::storeSources(std::span<std::filesystem::path const> sources) {
    auto sourceStrings = std::vector<std::string>{};
    sourceStrings.reserve(sources.size());
    for (auto const& source : sources) sourceStrings.push_back(source.string());
    m_json[SOURCES_KEY] = sourceStrings;
}

auto RevisionThree::sourcesContent() const -> std::vector<std::string> {
    return m_json.value(SOURCES_CONTENT_KEY, std::vector<std::string>{});
}

void RevisionThree::storeSourcesContent(std::span<std::string const> sourcesContent) {
    m_json[SOURCES_CONTENT_KEY] = sourcesContent;
}

auto RevisionThree::names() const -> std::vector<std::string> {
    return m_json.value(NAMES_KEY, std::vector<std::string>{});
}

void RevisionThree::storeNames(std::span<std::string const> names) { m_json[NAMES_KEY] = names; }

auto RevisionThree::fromJson(std::string_view json) -> RevisionThree {
    if (json.starts_with(PROTECTION_SEQUENCE)) {
        json = json.substr(PROTECTION_SEQUENCE.size());
    }
    return RevisionThree{Json::parse(json)};
}

auto RevisionThree::toJson(int indent) const -> std::string { return m_json.dump(indent); }

} // namespace SourceMap
