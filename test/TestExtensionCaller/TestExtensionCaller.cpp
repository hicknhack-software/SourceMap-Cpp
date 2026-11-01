// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

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

TEST_CASE("Caller graph survives serialization with root zero, siblings, and separate roots", "[Extension][Caller]") {
    auto const callers = CallerList{
        Caller{{"input://root", {1, 1}}, CallerIndex{-1}},
        Caller{{"template://outer", {2, 3}}, CallerIndex{0}},
        Caller{{"template://inner", {4, 5}}, CallerIndex{1}},
        Caller{{"input://other-root", {6, 7}}, CallerIndex{-1}},
        Caller{{"template://other", {8, 9}}, CallerIndex{3}},
        Caller{{"template://sibling", {10, 11}}, CallerIndex{0}},
    };
    auto data = Data{.extensionData = std::make_tuple(callers)};
    for (auto index = 0; index < static_cast<int>(callers.size()); ++index)
        data.entries.push_back(
            Entry{
                .generated = {index + 1, 1},
                .original = {SOURCE_ONE, {index + 1, 1}},
                .extensionData = std::make_tuple(CallerIndex{index}),
            });
    auto original = Mapping{std::move(data)};
    auto json = SourceMap::RevisionThree{};
    json.encodeMappings(original);
    auto decoded = Mapping{SourceMap::RevisionThree::fromJson(json.toJson()).decodedMappings<Data>()};
    REQUIRE(SourceMap::get<SourceMap::Extension::Caller>(decoded.data()) == callers);
    for (auto line = 1; line <= static_cast<int>(callers.size()); ++line) {
        auto before = original.findEntryByGenerated({line, 1});
        auto after = decoded.findEntryByGenerated({line, 1});
        REQUIRE(after);
        CHECK(SourceMap::buildCallerStack(decoded, after) == SourceMap::buildCallerStack(original, before));
    }
}

TEST_CASE("Caller indices stay aligned across empty mapping lines and unmapped segments", "[Extension][Caller]") {
    auto original = Mapping{Data{
        .entries =
            {
                Entry{
                    .generated = {2, 1},
                    .original = {SOURCE_ONE, {1, 1}},
                    .extensionData = std::make_tuple(CallerIndex{2})},
                Entry{.generated = {4, 1}},
                Entry{
                    .generated = {4, 5},
                    .original = {SOURCE_ONE, {1, 2}},
                    .extensionData = std::make_tuple(CallerIndex{1})},
                Entry{.generated = {4, 9}},
                Entry{
                    .generated = {7, 1},
                    .original = {SOURCE_ONE, {1, 3}},
                    .extensionData = std::make_tuple(CallerIndex{0})},
            },
        .extensionData = std::make_tuple(buildCallerList()),
    }};
    auto json = SourceMap::RevisionThree{};
    json.encodeMappings(original);
    auto decoded = Mapping{SourceMap::RevisionThree::fromJson(json.toJson()).decodedMappings<Data>()};
    REQUIRE(decoded.data().entries.size() == original.data().entries.size());
    for (auto index = size_t{}; index < original.data().entries.size(); ++index)
        CHECK(
            SourceMap::get<SourceMap::Extension::Caller>(decoded.data().entries[index]) ==
            SourceMap::get<SourceMap::Extension::Caller>(original.data().entries[index]));
}

TEST_CASE("Maps without a caller extension retain invalid caller indices", "[Extension][Caller]") {
    auto json = SourceMap::RevisionThree{};
    json.encodeMappings(Mapping{Data{.entries = buildEntries(), .extensionData = std::make_tuple(buildCallerList())}});
    json.json().erase("x_de_hicknhack_software_callstack");
    auto decoded = json.decodedMappings<Data>();
    CHECK(SourceMap::get<SourceMap::Extension::Caller>(decoded).empty());
    for (auto const& entry : decoded.entries)
        CHECK(SourceMap::get<SourceMap::Extension::Caller>(entry).value == SourceMap::InvalidCallerIndex);
}
