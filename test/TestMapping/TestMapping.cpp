// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

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
