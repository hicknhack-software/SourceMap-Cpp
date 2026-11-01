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
