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
#include <string>
#include <utility>

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
