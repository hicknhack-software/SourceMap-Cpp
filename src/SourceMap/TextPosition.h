// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include <string_view>

namespace SourceMap {

// Source map columns count UTF-16 code units; input text is UTF-8.
inline auto utf16Length(std::string_view text) -> int {
    auto length = 0;
    for (auto value : text) {
        auto byte = static_cast<unsigned char>(value);
        if ((byte & 0xc0) == 0x80) continue;
        length += byte >= 0xf0 ? 2 : 1;
    }
    return length;
}

} // namespace SourceMap
