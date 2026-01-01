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

#include <catch2/catch_test_macros.hpp>
#include <ranges>
#include <string>

using namespace SourceMap;

namespace {

auto const SOURCE_ONE = std::filesystem::path{"sourceOne"};
auto const SOURCE_TWO = std::filesystem::path{"sourceTwo"};
auto const GENERATED = std::filesystem::path{"generated"};
auto const SYMBOL = "symbol";

Entries<> buildEntries() {
    using Entry = SourceMap::Entry<>;
    return {
        Entry{.generated = {1, 1}},
        Entry{.generated = {1, 10}, .original = {.name = SOURCE_ONE, .position = {1, 1}}},
        Entry{.generated = {1, 15}},
        Entry{.generated = {2, 10}, .original = {.name = SOURCE_TWO, .position = {1, 1}}, .name = SYMBOL},
        Entry{.generated = {2, 1}},
        Entry{.generated = {2, 20}},
    };
}

} // namespace

TEST_CASE("Store and Restore", "[RevisionThree]") {
    using Mapping = SourceMap::Mapping<>;
    using Data = Mapping::Data;

    auto m = Mapping{SourceMap::Data{.entries = buildEntries()}};

    RevisionThree r3;
    REQUIRE(r3.version() == 3);

    r3.storeFile(GENERATED);
    REQUIRE(r3.file() == GENERATED);

    r3.encodeMappings(m);
    REQUIRE(r3.sources().size() == 2);
    REQUIRE(r3.names().size() == 1);

    auto& jsonRef = r3.json();
    (void)jsonRef;
    auto& constJsonRef = static_cast<RevisionThree const&>(r3).json();
    (void)constJsonRef;

    auto bytes = r3.toJson();

    auto rr = RevisionThree::fromJson(bytes);
    REQUIRE(rr.names() == r3.names());

    auto rm = Mapping{rr.decodedMappings<Data>()};
    REQUIRE(rm.data().entries.size() == m.data().entries.size());
    auto rmNames = rm.originalNames() | std::ranges::to<std::vector>();
    auto mNames = m.originalNames() | std::ranges::to<std::vector>();
    REQUIRE(rmNames == mNames);

    auto p1 = rm.findEntryByGenerated({2, 10});
    REQUIRE(p1->isValid());
    REQUIRE(p1->name == std::string(SYMBOL));
}
