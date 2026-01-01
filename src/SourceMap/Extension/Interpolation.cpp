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
#include "Interpolation.h"

#include "SourceMap/RevisionThree.h"
#include "SourceMap/intern/Base64VLQ.h"

namespace SourceMap::Extension::intern {

namespace {

using namespace std::string_view_literals;

auto const SEGMENT_DELIMITER = ',';
auto const INTERPOLATION_DELIMITER = ';';
auto const INTERPOLATIONS_KEY = "x_de_hicknhack_software_column_interpolation"sv;

auto encodeInterpolations(GeneratedLineInterpolationList const& interpolations) -> std::string {
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    auto encoded = std::string{};
    auto lastLine = 1;
    auto newLine = true;
    for (auto const& p : interpolations) {
        auto const line = std::get<0>(p);
        auto const interpolation = std::get<1>(p);

        auto const lineDiff = (line - lastLine);
        for (auto i = 0; i < lineDiff; ++i) {
            encoded.push_back(INTERPOLATION_DELIMITER);
        }

        if (lineDiff > 0) {
            lastLine = line;
            newLine = true;
        }

        if (lineDiff == 0 && newLine == false) {
            encoded.push_back(SEGMENT_DELIMITER);
        }

        Base64VLQ::encode(encoded, static_cast<int>(interpolation));
        newLine = false;
    }
    return encoded;
}

} // namespace

auto jsonDecodeInterpolationList(RevisionThree const& json) -> InterpolationList {
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    auto result = InterpolationList{};
    if (!json.json().contains(INTERPOLATIONS_KEY)) return result;

    auto const encoded = json.json()[INTERPOLATIONS_KEY].get<std::string>();
    auto begin = encoded.begin();
    auto const end = encoded.end();
    while (begin != end) {
        if (*begin == INTERPOLATION_DELIMITER || *begin == SEGMENT_DELIMITER) {
            ++begin;
            continue;
        }
        auto value = 0;
        Base64VLQ::decode(std::ref(begin), end, std::ref(value));
        result.emplace_back(static_cast<SourceMap::Interpolation>(value));
    }
    return result;
}

void jsonStoreInterpolations(
    std::reference_wrapper<RevisionThree> json, GeneratedLineInterpolationList const& interpolations) {
    auto const encodedInterpolations = encodeInterpolations(interpolations);
    json.get().json()[INTERPOLATIONS_KEY] = encodedInterpolations;
}

} // namespace SourceMap::Extension::intern
