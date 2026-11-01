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
#include <SourceMap/SourcePath.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Keyed source paths can be rebound without losing directory structure", "[sourcemap]") {
    CHECK(
        SourceMap::keyedPath("templates/cpp23/master.hpp.twofold", "templates", "template").string() ==
        "template://cpp23/master.hpp.twofold");
    auto roots =
        std::map<std::string, std::filesystem::path>{{"template", "/moved/templates"}, {"input", "/moved/inputs"}};
    CHECK(SourceMap::resolveKeyedPath("input://ntp/layout.txt", roots) == "/moved/inputs/ntp/layout.txt");
    CHECK_THROWS(SourceMap::keyedPath("elsewhere/file", "templates", "template"));
    CHECK_THROWS(SourceMap::resolveKeyedPath("input://../outside", roots));
    CHECK_THROWS(SourceMap::resolveKeyedPath("missing://file", roots));
}
