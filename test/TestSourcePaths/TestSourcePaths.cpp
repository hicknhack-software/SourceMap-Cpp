// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

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
