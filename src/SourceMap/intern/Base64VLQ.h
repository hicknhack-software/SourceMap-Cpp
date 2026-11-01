// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include <string>
#include <utility>
#include <functional>

/// Base 64 encoded variable length encoding for signed integer values
namespace SourceMap::intern::Base64VLQ {

using string = std::string;
using string_ref = std::reference_wrapper<string>;
using iterator = string::iterator;
using iterator_ref = std::reference_wrapper<iterator>;
using const_iterator = string::const_iterator;
using const_iterator_ref = std::reference_wrapper<const_iterator>;
using int_ref = std::reference_wrapper<int>;

/// stores the value into the string
void encode(string_ref str, int value);

/// @returns true, if result contains the decoded value otherwise result keeps it's initial value
auto decode(const_iterator_ref begin, const_iterator end, int_ref result) -> bool;

/// @returns the decoded value otherwise error value
auto decode(const_iterator_ref begin, const_iterator end, int error) -> int;

} // namespace SourceMap::intern::Base64VLQ
