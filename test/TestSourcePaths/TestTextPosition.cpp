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
