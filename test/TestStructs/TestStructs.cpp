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
#include "SourceMap/Entry.h"
#include "SourceMap/FilePosition.ostream.h"
#include "SourceMap/Position.ostream.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("Structs Position", "[Structs]") {
    using namespace SourceMap;

    auto p1 = Position{};
    REQUIRE(p1.isValid() == false);

    auto p2 = Position{.line = 3, .column = 4};
    REQUIRE(p2.isValid() == true);
    REQUIRE(p2 == Position(3, 4));

    REQUIRE(p1 != p2);
}

auto const TEST_STR1 = std::filesystem::path{"hello"};

TEST_CASE("Structs FilePosition", "[Structs]") {
    using namespace SourceMap;

    auto p1 = FilePosition{};
    REQUIRE(p1.isValid() == false);

    auto p2 = FilePosition{TEST_STR1, {3, 4}};
    REQUIRE(p2.isValid() == true);
    REQUIRE(p2 == FilePosition(TEST_STR1, {3, 4}));

    REQUIRE(p1 != p2);
}

TEST_CASE("Structs Entry", "[Structs]") {
    using namespace SourceMap;

    using Entry = SourceMap::Entry<>;

    auto p1 = Entry{};
    REQUIRE(p1.isValid() == false);

    auto p2 = Entry{Position{3, 4}};
    REQUIRE(p2.isValid() == true);
    REQUIRE(p2.generated == Position(3, 4));

    auto p3 = Entry{Position{3, 4}, FilePosition{TEST_STR1, Position{6, 4}}};
    REQUIRE(p3.isValid() == true);
    REQUIRE(p3.generated == Position(3, 4));
    REQUIRE(p3.original == FilePosition(TEST_STR1, {6, 4}));

    auto p4 = Entry{Position{3, 4}, {TEST_STR1, {6, 4}}, "hello"};
    REQUIRE(p4.isValid() == true);
    REQUIRE(p4.generated == Position(3, 4));
    REQUIRE(p4.original == FilePosition(TEST_STR1, {6, 4}));
    REQUIRE(p4.name == "hello");
}
