// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "SourceMap/Data.h"

namespace SourceMap {

template<typename... ExtensionTypes>
auto Data<ExtensionTypes...>::addEntry(typename Data::Entry const& entry) -> bool {
    if (entries.empty()) {
        entries.push_back(entry);
        return false;
    }
    auto const& lastEntry = entries.back();

    auto const isLess = PositionLess::operator()(lastEntry.generated, entry.generated);
    auto override = false;
    if (isLess) {
        entries.push_back(entry);
    }
    else {
        entries[entries.size() - 1] = entry;
        override = true;
    }
    return override;
}

} // namespace SourceMap
