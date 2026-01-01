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
#include "SourceMap/Extension/Caller.h"

#include "SourceMap/Entry.h"
#include "SourceMap/FilePosition.ostream.h"
#include "SourceMap/Mapping.h"
#include "SourceMap/RevisionThree.h"

#include <catch2/catch_test_macros.hpp>
#include <ranges>
#include <string>
#include <tuple>

namespace {

using Caller = SourceMap::Caller;
using CallerList = SourceMap::CallerList;
using CallerStack = SourceMap::CallerStack;
using CallerIndex = SourceMap::CallerIndex;
using FilePosition = SourceMap::FilePosition;
using Mapping = SourceMap::Mapping<SourceMap::Extension::Caller>;
using Entry = Mapping::Entry;
using Data = Mapping::Data;
using EntryList = Mapping::Entries;

auto const SOURCE_ONE = std::filesystem::path{"sourceOne"};
auto const SOURCE_TWO = std::filesystem::path{"sourceTwo"};
auto const GENERATED = std::filesystem::path{"generated"};
auto const SYMBOL = "symbol";

EntryList buildEntries() {
    return {
        Entry{.generated = {1, 1}},
        Entry{
            .generated = {1, 10},
            .original = FilePosition{.name = SOURCE_ONE, .position = {1, 1}},
            .extensionData = std::make_tuple(CallerIndex{0})},
        Entry{.generated = {1, 15}},
        Entry{
            .generated = {2, 10},
            .original = FilePosition{.name = SOURCE_TWO, .position = {1, 1}},
            .name = SYMBOL,
            .extensionData = std::make_tuple(CallerIndex{2})},
        Entry{.generated = {2, 1}},
        Entry{.generated = {2, 20}},
    };
}

CallerList buildCallerList() {
    return {
        Caller{.original = {.name = SOURCE_TWO, .position = {20, 1}}, .parentIndex = CallerIndex{-1}},
        Caller{.original = {.name = SOURCE_TWO, .position = {40, 1}}, .parentIndex = CallerIndex{-1}},
        Caller{.original = {.name = SOURCE_TWO, .position = {40, 10}}, .parentIndex = CallerIndex{1}},
    };
}

} // namespace

TEST_CASE("Caller Extension Values", "[Extension][Caller]") {
    auto p1 = CallerIndex{};
    REQUIRE(p1.value == SourceMap::InvalidCallerIndex);

    auto p2 = CallerIndex{99};
    REQUIRE(p2.value == 99);

    auto c1 = Caller{};
    REQUIRE(!c1.isValid());

    auto c2 = Caller{{SOURCE_ONE, {3, 4}}};
    REQUIRE(c2.isValid());
    REQUIRE(c2.original == (SourceMap::FilePosition{SOURCE_ONE, {3, 4}}));
    REQUIRE(c2.parentIndex.value == SourceMap::InvalidCallerIndex);

    auto c3 = Caller{{SOURCE_ONE, {3, 4}}, CallerIndex{2}};
    REQUIRE(c3.parentIndex.value == 2);
}

auto const TEST_STR1 = std::filesystem::path{"hello"};

TEST_CASE("Caller Extension Entry", "[Extension][Caller]") {
    using CallerExt = SourceMap::Extension::Caller;
    using SourceMap::get;

    auto p2 = Entry{{3, 4}};
    REQUIRE(get<CallerExt>(p2).value == SourceMap::InvalidCallerIndex);

    auto p4 = Entry{{3, 4}, {TEST_STR1, {6, 4}}, "hello", std::make_tuple(CallerIndex{32})};
    REQUIRE(get<CallerExt>(p4).value == 32);
}

TEST_CASE("Caller Extension Mapping", "[Extension][Caller]") {
    using FilePosition = SourceMap::FilePosition;

    auto map = Mapping{{buildEntries(), std::make_tuple(buildCallerList())}};
    auto e1 = map.findEntryByGenerated({1, 5});
    REQUIRE(e1 != nullptr);

    auto p1 = SourceMap::buildCallerStack(map.data(), e1);
    REQUIRE(p1.empty());

    auto e2 = map.findEntryByGenerated({2, 13});
    REQUIRE(e2 != nullptr);

    auto p2 = SourceMap::buildCallerStack(map.data(), e2);
    REQUIRE(p2.size() == 2u);
    REQUIRE(p2[0] == FilePosition(SOURCE_TWO, {40, 10}));
}

TEST_CASE("Caller Extension RevisionThree", "[Extension][Caller]") {
    using FilePosition = SourceMap::FilePosition;
    using RevisionThree = SourceMap::RevisionThree;

    auto m = Mapping{SourceMap::Data{
        .entries = buildEntries(),
        .extensionData = std::make_tuple(buildCallerList()),
    }};

    auto r3 = RevisionThree{};
    REQUIRE(r3.version() == 3);

    r3.storeFile(GENERATED);
    REQUIRE(r3.file() == GENERATED);

    r3.encodeMappings(m);
    REQUIRE(r3.sources().size() == 2);
    REQUIRE(r3.names().size() == 1);

    auto bytes = r3.toJson();

    auto rr = RevisionThree::fromJson(bytes);
    REQUIRE(rr.names() == r3.names());

    auto rm = Mapping{rr.decodedMappings<Data>()};
    REQUIRE(rm.data().entries.size() == m.data().entries.size());
    auto rmNames = rm.originalNames() | std::ranges::to<std::vector>();
    auto mNames = m.originalNames() | std::ranges::to<std::vector>();
    REQUIRE(rmNames == mNames);

    auto const* e2 = rm.findEntryByGenerated({2, 13});
    REQUIRE(e2 != nullptr);

    auto p2 = SourceMap::buildCallerStack(rm, e2);
    REQUIRE(p2.size() == 2u);
    REQUIRE(p2[0] == FilePosition(SOURCE_TWO, {40, 10}));
}
