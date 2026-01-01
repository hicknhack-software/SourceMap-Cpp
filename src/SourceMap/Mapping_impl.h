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
#include "SourceMap/Mapping.h"

#include <algorithm>
#include <cassert>

namespace SourceMap {

template<typename... ExtensionTypes>
class Mapping<ExtensionTypes...>::Private {
public:
    using Entry = SourceMap::Entry<ExtensionTypes...>;
    using EntryList = SourceMap::Entries<ExtensionTypes...>;
    using Data = SourceMap::Data<ExtensionTypes...>;
    using Extensions = SourceMap::Extensions<ExtensionTypes...>;

    struct EntryGeneratedLess {
        using is_transparent = void;
        static constexpr auto operator()(Entry const& l, Entry const& r) -> bool {
            return PositionLess::operator()(l.generated, r.generated);
        }
        static constexpr auto operator()(Entry const& l, Position const& r) -> bool {
            return PositionLess::operator()(l.generated, r);
        }
        static constexpr auto operator()(Position const& l, Entry const& r) -> bool {
            return PositionLess::operator()(l, r.generated);
        }
    };
    struct EntryGeneratedNotLess {
        using is_transparent = void;
        static constexpr auto operator()(Entry const& l, Entry const& r) -> bool {
            return !PositionLess::operator()(l.generated, r.generated);
        }
        static constexpr auto operator()(Entry const& l, Position const& r) -> bool {
            return !PositionLess::operator()(l.generated, r);
        }
        static constexpr auto operator()(Position const& l, Entry const& r) -> bool {
            return !PositionLess::operator()(l, r.generated);
        }
    };

    Private(Data data)
        : m_data(std::move(data)) {
        buildGeneratedSorted();
    }

    auto data() const -> Data const& { return m_data; }

    auto originalNames() const -> std::span<std::filesystem::path const> {
        if (m_originalNames.empty()) buildOriginalNames();
        return m_originalNames;
    }

    auto findEntryByGenerated(Position const& position) -> Entry const* {
        if (auto it =
                std::upper_bound(m_data.entries.rbegin(), m_data.entries.rend(), position, EntryGeneratedNotLess{});
            it != m_data.entries.rend()) {
            return &*it;
        }
        return nullptr; // nothing found
    }

    void buildGeneratedSorted() { std::sort(m_data.entries.begin(), m_data.entries.end(), EntryGeneratedLess{}); }

    void buildOriginalNames() const {
        assert(m_originalNames.empty());
        // avoid using a std::set - the files list should be stable
        auto add = [&](std::filesystem::path const& name) {
            if (name.empty()) return;
            if (std::none_of(m_originalNames.begin(), m_originalNames.end(), [&](std::filesystem::path const& v) {
                    return v == name;
                })) {
                m_originalNames.push_back(name);
            }
        };
        for (auto& entry : m_data.entries) add(entry.original.name);
        Extensions::collectFileNames(m_data, add);
    }

private:
    Data m_data;
    mutable std::vector<std::filesystem::path> m_originalNames;
};

template<typename... ExtensionTypes>
Mapping<ExtensionTypes...>::Mapping(Data data)
    : m_private(new Private(std::move(data))) {}

template<typename... ExtensionTypes>
auto Mapping<ExtensionTypes...>::data() const -> typename Mapping<ExtensionTypes...>::Data const& {
    return m_private->data();
}

template<typename... ExtensionTypes>
auto Mapping<ExtensionTypes...>::entriesSortedToGeneratedPosition() const ->
    typename Mapping<ExtensionTypes...>::Entries const& {
    return m_private->data().entries;
}

template<typename... ExtensionTypes>
auto Mapping<ExtensionTypes...>::originalNames() const -> std::span<std::filesystem::path const> {
    return m_private->originalNames();
}

template<typename... ExtensionTypes>
auto Mapping<ExtensionTypes...>::findEntryByGenerated(Position const& position) const ->
    typename Mapping<ExtensionTypes...>::Entry const* {
    if (!position.isValid()) return nullptr;
    return m_private->findEntryByGenerated(position);
}

} // namespace SourceMap
