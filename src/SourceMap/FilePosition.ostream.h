// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "FilePosition.h"

#include <ostream>

namespace SourceMap {

auto operator<<(std::ostream& out, FilePosition const& fp) -> std::ostream& {
    return out << "P{.file=\"" << fp.name << "\" .line=" << fp.position.line << " .column=" << fp.position.column
               << "}";
}

} // namespace SourceMap
