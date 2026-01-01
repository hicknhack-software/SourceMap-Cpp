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
#include "SourceMap/Mapping.h"
#include "SourceMap/Position.ostream.h"

#include <catch2/catch_test_macros.hpp>

using namespace SourceMap;

namespace {

auto const SOURCE_ONE = std::filesystem::path{"sourceOne"};
auto const SOURCE_TWO = std::filesystem::path{"sourceTwo"};

Entries<> buildEntries() {
    using Entry = SourceMap::Entry<>;
    return {
        Entry{.generated = {1, 1}},
        Entry{.generated = {1, 10}, .original = {.name = SOURCE_ONE, .position = {1, 1}}},
        Entry{.generated = {1, 15}},
        Entry{.generated = {2, 10}, .original = {.name = SOURCE_TWO, .position = {1, 1}}},
        Entry{.generated = {2, 1}},
        Entry{.generated = {2, 20}},
    };
}

} // namespace

TEST_CASE("Mapping Construction", "[Mapping]") {
    using Mapping = SourceMap::Mapping<>;
    auto empty = Mapping();
    (void)empty;

    auto entries = buildEntries();
    auto moved = Mapping{SourceMap::Data{.entries = buildEntries()}};
    REQUIRE(moved.data().entries.size() == entries.size());
    REQUIRE(moved.originalNames().size() == 2);

    auto copied = Mapping{SourceMap::Data{.entries = entries}};
    REQUIRE(copied.data().entries.size() == entries.size());
    REQUIRE(copied.originalNames().size() == 2);

    auto sorted = copied.data().entries;
    REQUIRE(sorted[3].generated == entries[4].generated);
}

TEST_CASE("Mapping Find Entry", "[Mapping]") {
    using Mapping = SourceMap::Mapping<>;

    auto map = Mapping{SourceMap::Data{.entries = buildEntries()}};
    auto p1 = map.findEntryByGenerated({1, 9});
    REQUIRE(p1 != nullptr);
    REQUIRE(p1->generated == Position(1, 1));

    auto p2 = map.findEntryByGenerated({1, 10});
    REQUIRE(p2 != nullptr);
    REQUIRE(p2->generated == Position(1, 10));

    auto p3 = map.findEntryByGenerated({10, 100});
    REQUIRE(p3 != nullptr);
    REQUIRE(p3->generated == Position(2, 20));
}
