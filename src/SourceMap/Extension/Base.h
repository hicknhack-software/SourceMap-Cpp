// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <utility>
#include <functional>

namespace SourceMap {

class RevisionThree;

namespace Extension {

template<class T>
concept isVoidOrSized = std::is_same_v<T, void> || sizeof(T) > 0;

template<class T, class Data>
concept ValidExtensionFor =
    requires(Data const& data, Data& mutData, RevisionThree& mutR3, RevisionThree const& r3, bool success) {
        isVoidOrSized<typename T::EntryData>;
        isVoidOrSized<typename T::MapData>;
        T::template jsonEncode<Data>(data, std::ref(mutR3));
        success = T::template jsonDecode<Data>(std::ref(mutData), r3);
        T::collectFileNames(data, [](std::string const&) {});
    };

/// Base Schema of a valid SourceMap extension
struct Base {
    using EntryData = void; ///< additional Entry data members - default to void if none
    using MapData = void; ///< additional Mapping data members - default to void if none

    // /// Mapping encode function
    // template< typename Data >
    // static void jsonEncode(const Data&, std::reference_wrapper<RevisionThree>);

    // /// @returns false if decoding failed
    // template< typename Mapping >
    // static auto jsonDecode(std::reference_wrapper<Mapping>, const RevisionThree&) -> bool;

    /// Invoke Callback with additional FileNames
    ///
    /// * optional (default will not report any file names
    template<typename Data, typename Callback>
    static void collectFileNames(Data const&, Callback&&) {}
};

} // namespace Extension
} // namespace SourceMap
