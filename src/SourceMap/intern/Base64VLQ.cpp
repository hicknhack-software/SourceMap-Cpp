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
#include "Base64VLQ.h"

#include <array>
#include <assert.h>
#include <cstdint>
#include <limits>
#include <vector>

namespace SourceMap::intern::Base64VLQ {
namespace {

// A single base 64 digit can contain 6 bits of data. For the base 64 variable
// length quantities we use in the source map spec, the first bit is the sign,
// the next four bits are the actual value, and the 6th bit is the
// continuation bit. The continuation bit tells us whether there are more
// digits in this value following this digit.
//
//   Continuation
//   |    Sign
//   |    |
//   V    V
//   101011
constexpr auto VLQ_BASE_SHIFT{5};
constexpr auto VLQ_BASE{1 << VLQ_BASE_SHIFT};
constexpr auto VLQ_BASE_MASK{VLQ_BASE - 1};
constexpr auto VLQ_CONTINUATION_BIT{VLQ_BASE};

constexpr auto int_to_char = std::string_view{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

constexpr auto base64_encode(std::uint8_t digit) -> string::value_type {
    assert(digit < int_to_char.size());
    return int_to_char.at(digit);
}

constexpr auto invalid_digit = std::numeric_limits<std::uint8_t>::max();
constexpr auto char_to_value = []() -> std::array<uint8_t, 128> {
    auto result = std::array<uint8_t, 128>{};
    for (auto& val : result) val = invalid_digit;
    auto value = 0;
    for (auto chr : int_to_char) result[static_cast<size_t>(chr)] = value++;
    return result;
}();

constexpr auto base64_decode(string::value_type chr) -> int {
    auto const index = static_cast<unsigned char>(chr);
    if (index >= char_to_value.size()) return invalid_digit;
    return char_to_value.at(index);
}

/**
 * Converts from a two-complement value to a value where the sign bit is
 * is placed in the least significant bit. For example, as decimals:
 *   1 becomes 2 (10 binary), -1 becomes 3 (11 binary)
 *   2 becomes 4 (100 binary), -2 becomes 5 (101 binary)
 */
constexpr auto toVLQSigned(int aValue) -> int { return aValue < 0 ? ((-aValue) << 1) + 1 : (aValue << 1) + 0; }

/**
 * Converts to a two-complement value from a value where the sign bit is
 * is placed in the least significant bit. For example, as decimals:
 *   2 (10 binary) becomes 1, 3 (11 binary) becomes -1
 *   4 (100 binary) becomes 2, 5 (101 binary) becomes -2
 */
constexpr auto fromVLQSigned(int aValue) -> int {
    auto const isNegative = (aValue & 1) == 1;
    auto const shifted = aValue >> 1;
    return isNegative ? -shifted : shifted;
}

} // namespace

void encode(string_ref str, int value) {
    auto vlq = toVLQSigned(value);
    do {
        auto digit = vlq & VLQ_BASE_MASK;
        vlq = vlq >> VLQ_BASE_SHIFT;
        if (vlq > 0) digit = digit | VLQ_CONTINUATION_BIT;
        str.get().push_back(base64_encode(digit));
    } while (vlq > 0);
}

auto decode(const_iterator_ref begin, const_iterator end, int_ref result) -> bool {
    if (begin.get() == end) return false; // nothing to read
    auto const digit = base64_decode(*begin.get());
    if (invalid_digit == digit) return false; // no valid digit
    ++begin.get();
    auto hasNext = (digit & VLQ_CONTINUATION_BIT) != 0;
    auto decoded = digit & VLQ_BASE_MASK;
    auto shift = 0;
    while (hasNext) {
        if (begin.get() == end) return false; // reached end (encoding error!)
        auto const digit = base64_decode(*begin.get());
        ++begin.get();
        hasNext = (digit & VLQ_CONTINUATION_BIT) != 0;
        decoded += (digit & VLQ_BASE_MASK) << (shift += VLQ_BASE_SHIFT);
    }
    result.get() = fromVLQSigned(decoded);
    return true;
}

auto decode(const_iterator_ref begin, const_iterator end, int error) -> int {
    decode(begin, end, std::ref(error));
    return error;
}

} // namespace SourceMap::intern::Base64VLQ
