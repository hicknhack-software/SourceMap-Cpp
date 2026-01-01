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

using Interpolation = SourceMap::Interpolation;
using FilePosition = SourceMap::FilePosition;
using Mapping = SourceMap::Mapping<SourceMap::Extension::Interpolation>;
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
            .extensionData = std::make_tuple(Interpolation::OneToOne)},
        Entry{.generated = {1, 15}},
        Entry{
            .generated = {2, 10},
            .original = FilePosition{.name = SOURCE_TWO, .position = {1, 1}},
            .name = SYMBOL,
            .extensionData = std::make_tuple(Interpolation::OneToOne)},
        Entry{.generated = {2, 1}},
        Entry{.generated = {2, 20}},
    };
}

} // namespace

TEST_CASE("Extension Interpolation Value", "[Extension][Interpolation]") {
    auto p1 = Interpolation{};
    REQUIRE(p1 == Interpolation::None);

    auto p2 = Interpolation::OneToOne;
    REQUIRE(p2 == Interpolation::OneToOne);
}

auto const TEST_STR1 = std::filesystem::path{"hello"};

TEST_CASE("Extension Interpolation Entry", "[Extension][Interpolation]") {
    using InterpolationExt = SourceMap::Extension::Interpolation;
    using SourceMap::get;

    auto p2 = Entry{{3, 4}};
    REQUIRE(get<InterpolationExt>(p2) == Interpolation::None);

    auto p4 = Entry{{3, 4}, {TEST_STR1, {6, 4}}, "hello", std::make_tuple(Interpolation::OneToOne)};
    REQUIRE(get<InterpolationExt>(p4) == Interpolation::OneToOne);
}

TEST_CASE("Extension Interpolation Mapping", "[Extension][Interpolation]") {
    using FilePosition = SourceMap::FilePosition;

    auto map = Mapping{SourceMap::Data{.entries = buildEntries()}};
    auto p1 = SourceMap::getOriginalPositionFromGenerated(map, {1, 5});
    REQUIRE(!p1.isValid());

    auto p2 = SourceMap::getOriginalPositionFromGenerated(map, {1, 13});
    REQUIRE(p2.isValid());
    REQUIRE(p2 == FilePosition(SOURCE_ONE, {1, 4}));
}

TEST_CASE("Extension Interpolation RevisionThree", "[Extension][Interpolation]") {
    using FilePosition = SourceMap::FilePosition;
    using RevisionThree = SourceMap::RevisionThree;

    auto m = Mapping{SourceMap::Data{.entries = buildEntries()}};

    RevisionThree r3;
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

    auto p2 = SourceMap::getOriginalPositionFromGenerated(rm, {1, 13});
    REQUIRE(p2.isValid());
    REQUIRE(p2 == FilePosition(SOURCE_ONE, {1, 4}));
}
