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
#include <filesystem>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>

namespace SourceMap {

// The root is an ancestor, so moving the source tree only changes its key binding.
inline auto keyedPath(std::filesystem::path const& path, std::filesystem::path const& root, std::string_view key)
    -> std::filesystem::path {
    if (path.empty()) return {};
    if (key.empty() ||
        key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") != key.npos)
        throw std::runtime_error("Invalid source path key: " + std::string{key});
    auto relative = std::filesystem::absolute(path).lexically_normal().lexically_relative(
        std::filesystem::absolute(root.empty() ? "." : root).lexically_normal());
    if (relative.empty() || relative.is_absolute() || *relative.begin() == "..")
        throw std::runtime_error("Source path is outside its root: " + path.string());
    return std::string{key} + "://" + relative.generic_string();
}

inline auto resolveKeyedPath(std::string_view uri, std::map<std::string, std::filesystem::path> const& roots)
    -> std::filesystem::path {
    auto separator = uri.find("://");
    if (separator == uri.npos) throw std::runtime_error("Source has no path key: " + std::string{uri});
    auto key = std::string{uri.substr(0, separator)};
    auto relative = std::filesystem::path{uri.substr(separator + 3)}.lexically_normal();
    if (relative.empty() || relative.is_absolute() || *relative.begin() == "..")
        throw std::runtime_error("Invalid keyed source path: " + std::string{uri});
    auto root = roots.find(key);
    if (root == roots.end()) throw std::runtime_error("No root provided for source key: " + key);
    return (root->second / relative).lexically_normal();
}

} // namespace SourceMap
