// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SourceMap/Extension/Base.h"

#include "SourceMap/FilePosition.h"

namespace SourceMap {

template<typename... ExtensionTypes>
class Mapping;

/// Flags wheter source interpolation is possible
enum class Interpolation {
    None, ///< no interpolation possible
    OneToOne ///< 1:1 from source - interpolate the columns
};

/// @returns invalid Position for invalid position
/// interpolated position if interpolation is allowed
/// start of the entry before if no interpolation is allowed
template<typename... ExtensionTypes>
auto getOriginalPositionFromGenerated(Mapping<ExtensionTypes...> const& mapping, Position const& position)
    -> FilePosition;

namespace Extension {

/// This implements the Extension interface for the Interpolation
struct Interpolation : Base {
    using EntryData = SourceMap::Interpolation;

    /// @brief Adds x_de_hicknhack_software_column_interpolation JSON entry
    /// It encodes the value of the Interpolation Flag for each mapping.
    /// * Each caller entry is seperated by ';'
    /// * Segments in a line are separated by ','
    /// * Base64 VLQ encoded value of the Interpolation flag
    template<typename Data>
    static void jsonEncode(Data const&, std::reference_wrapper<RevisionThree>);

    /// @brief Decodes x_de_hicknhack_software_column_interpolation JSON entry
    /// If it's missing no interpolations are done (no error is raised)
    template<typename Data>
    static auto jsonDecode(std::reference_wrapper<Data>, RevisionThree const&) -> bool;
};

} // namespace Extension
} // namespace SourceMap

#include "Interpolation_impl.h"
