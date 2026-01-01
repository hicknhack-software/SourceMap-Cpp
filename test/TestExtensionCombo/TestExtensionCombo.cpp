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
#include "SourceMap/Extension/Interpolation.h"

#include "SourceMap/Entry.h"
#include "SourceMap/FilePosition.ostream.h"
#include "SourceMap/Mapping.h"
#include "SourceMap/RevisionThree.h"

#include <catch2/catch_test_macros.hpp>
#include <ranges>
#include <string>
#include <tuple>

namespace {

using ExtInterpolation = SourceMap::Extension::Interpolation;
using Interpolation = SourceMap::Interpolation;

using ExtCaller = SourceMap::Extension::Caller;
using Caller = SourceMap::Caller;
using CallerList = SourceMap::CallerList;
using CallerIndex = SourceMap::CallerIndex;
using FilePosition = SourceMap::FilePosition;

using Mapping = SourceMap::Mapping<ExtInterpolation, ExtCaller>;
using Data = Mapping::Data;
using Entry = Mapping::Entry;
using EntryList = Mapping::Entries;

auto const SOURCE_ONE = std::filesystem::path{"sourceOne"};
auto const SOURCE_TWO = std::filesystem::path{"sourceTwo"};
auto const GENERATED = std::filesystem::path{"generated"};
auto const SYMBOL = "symbol";

auto buildEntries() -> EntryList {
    return {
        Entry{.generated = {1, 1}},
        Entry{
            .generated = {1, 10},
            .original = FilePosition{.name = SOURCE_ONE, .position = {1, 1}},
            .extensionData = std::make_tuple(Interpolation::OneToOne, CallerIndex{0}),
        },
        Entry{.generated = {1, 15}},
        Entry{
            .generated = {2, 10},
            .original = FilePosition{.name = SOURCE_TWO, .position = {1, 1}},
            .name = SYMBOL,
            .extensionData = std::make_tuple(Interpolation::OneToOne, CallerIndex{2}),
        },
        Entry{.generated = {2, 1}},
        Entry{.generated = {2, 20}},
    };
}

auto buildCallerList() -> CallerList {
    return {
        Caller{.original = {.name = SOURCE_TWO, .position = {20, 1}}, .parentIndex = CallerIndex{-1}},
        Caller{.original = {.name = SOURCE_TWO, .position = {40, 1}}, .parentIndex = CallerIndex{-1}},
        Caller{.original = {.name = SOURCE_TWO, .position = {40, 10}}, .parentIndex = CallerIndex{1}},
    };
}

} // namespace

auto const TEST_STR1 = std::filesystem::path{"hello"};

TEST_CASE("Extension Combo Entry", "[Extension][Combo]") {
    using SourceMap::get;

    auto p2 = Entry{.generated = {3, 4}};
    REQUIRE(get<ExtCaller>(p2).value == SourceMap::InvalidCallerIndex);
    REQUIRE(get<ExtInterpolation>(p2) == Interpolation::None);

    auto p4 = Entry{
        .generated = {3, 4},
        .original = {TEST_STR1, {6, 4}},
        .name = "hello",
        .extensionData = std::make_tuple(Interpolation::OneToOne, CallerIndex{32}),
    };
    REQUIRE(get<ExtCaller>(p4).value == 32);
    REQUIRE(get<ExtInterpolation>(p4) == Interpolation::OneToOne);
}

TEST_CASE("Extension Combo Mapping", "[Extension][Combo]") {
    using FilePosition = SourceMap::FilePosition;

    auto const map = Mapping{{buildEntries(), std::make_tuple(buildCallerList())}};
    auto e1 = map.findEntryByGenerated({1, 5});
    REQUIRE(e1 != nullptr);

    auto p1 = SourceMap::buildCallerStack(map.data(), e1);
    REQUIRE(p1.empty());

    auto e2 = map.findEntryByGenerated({2, 13});
    REQUIRE(e2 != nullptr);

    auto p2 = SourceMap::buildCallerStack(map.data(), e2);
    REQUIRE(p2.size() == 2u);
    REQUIRE(p2[0] == FilePosition(SOURCE_TWO, {40, 10}));

    auto o1 = SourceMap::getOriginalPositionFromGenerated(map, {1, 5});
    REQUIRE(!o1.isValid());

    auto o2 = SourceMap::getOriginalPositionFromGenerated(map, {1, 13});
    REQUIRE(o2.isValid());
    REQUIRE(o2 == FilePosition(SOURCE_ONE, {1, 4}));
}

TEST_CASE("Extension Combo RevisionThree", "[Extension][Combo]") {
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

    auto p2 = SourceMap::buildCallerStack(rm.data(), e2);
    REQUIRE(p2.size() == 2u);
    REQUIRE(p2[0] == FilePosition(SOURCE_TWO, {40, 10}));
}
