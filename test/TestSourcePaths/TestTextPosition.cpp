// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#include <SourceMap/TextPosition.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("UTF-8 input can be measured in UTF-16 code units", "[text-position]") {
    struct Case {
        std::string_view utf8;
        int codeUnits;
    };
    auto data = GENERATE(Case{"", 0}, Case{"ASCII", 5}, Case{"é", 1}, Case{"中", 1}, Case{"😀", 2}, Case{"é😀 end", 7});
    CHECK(SourceMap::utf16Length(data.utf8) == data.codeUnits);
}
