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
#include "SourceMap/Mapping.h"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <span>
#include <string>
#include <vector>

namespace SourceMap {

/// @brief Implementation of the "Source Map Revision 3 Proposal" from Mozilla & Google
/// https://docs.google.com/document/d/1U1RGAehQwRypUTovF1KRlpiOFze0b-_2gc6fAH0KY0k/edit?pli=1#heading=h.1ce2c87bpj24
/// You should only add additional values according to the standard!
class RevisionThree {
public:
    using Json = nlohmann::json;

    /// Creates the construct with version 3
    RevisionThree();

    explicit RevisionThree(Json source);

    /// @returns stored version value
    auto version() const -> int;

    /// @returns stored generated file name (might be empty)
    auto file() const -> std::filesystem::path;
    void storeFile(std::filesystem::path const& file); ///< stores the name of the generated file

    /// @returns stored base url/path for all sources
    auto sourceRoot() const -> std::filesystem::path;
    void storeSourceRoot(std::filesystem::path const& sourceRoot); ///< stores base url/path for all sources

    /// @returns stored names for all sources relative to sourceRoot (indexed from mappings)
    auto sources() const -> std::vector<std::filesystem::path>;
    void storeSources(
        std::span<std::filesystem::path const> sources); ///< stores names for all sources relative to sourceRoot
                                                         ///< (indexed from mappings, called by setMapping)

    /// @returns stored content for all sources (has to correlate stored names for the sources or stay empty)
    auto sourcesContent() const -> std::vector<std::string>;
    void storeSourcesContent(
        std::span<std::string const> sourcesContent); ///< stores content for all sources (has to correlate stored
                                                      ///< names for the sources or stay empty)

    /// @returns stored (symbol) names (indexed from the mappings)
    auto names() const -> std::vector<std::string>;
    void storeNames(std::span<std::string const> names); ///< stores the (symbol) names (indexed from the mappings,
                                                         ///< called by setMapping)

    /// @returns decoded version of mappings (includes sources, names and extensions)
    template<typename Data>
    auto decodedMappings() const -> Data;

    /// stores the encoded mappings together with sources, names and interpolations
    template<typename Mapping>
    void encodeMappings(Mapping const&);

    /// creates the store from a JSON representations (also removes protections)
    static auto fromJson(std::string_view json) -> RevisionThree;

    /// creates a JSON representation of the stored data
    auto toJson(int indent = -1) const -> std::string;

    auto json(this auto&& self) -> decltype(*&self.m_json) { return self.m_json; }

private:
    Json m_json;
};

} // namespace SourceMap

#include "RevisionThree_impl.h"
