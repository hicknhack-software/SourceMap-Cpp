// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "Position.h"

#include <ostream>

namespace SourceMap {

auto operator<<(std::ostream& out, Position const& position) -> std::ostream& {
    return out << "P{.line=" << position.line << " .column=" << position.column << "}";
}

} // namespace SourceMap
