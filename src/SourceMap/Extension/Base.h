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
