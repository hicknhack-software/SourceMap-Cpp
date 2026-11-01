// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace SourceMap {

/// @brief line and column position in a text file
/// the first line in a file is 1 (this is not enforced)
/// the first column on a line is 1 (0 marks an invalid position)
/// this is a pure data container structure
struct Position {
    auto operator==(Position const&) const -> bool = default;

    /// checks for a valid position
    constexpr auto isValid() const -> bool { return 0 != column; }

    int line{};
    int column{};
};

struct PositionLess {
    static constexpr auto operator()(Position const& l, Position const& r) -> bool {
        auto const lineDiff = r.line - l.line;
        if (0 != lineDiff) return lineDiff > 0;

        auto const columnDiff = r.column - l.column;
        return columnDiff > 0;
    }
};

} // namespace SourceMap
