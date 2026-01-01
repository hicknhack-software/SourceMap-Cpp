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
#pragma once

#include "SourceMap/Position.h"

#include <filesystem>
#include <string>

namespace SourceMap {

/// @brief combines a (file) name and a position in that file
/// an empty name marks the position invalid (think of no source is known)
/// this is a pure data container structure
struct FilePosition {
    std::filesystem::path name;
    Position position;

    auto operator==(FilePosition const&) const -> bool = default;

    /// checks for valid name and position
    auto isValid() const -> bool { return !name.empty() && position.isValid(); }
};

struct FilePositionLess {
    static auto operator()(FilePosition const& l, FilePosition const& r) -> bool {
        if (l.name != r.name) return l.name < r.name;
        return PositionLess::operator()(l.position, r.position);
    }
};

} // namespace SourceMap
