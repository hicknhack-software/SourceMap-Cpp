// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

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
