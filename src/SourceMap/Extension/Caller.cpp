// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#include "Caller.h"

#include "SourceMap/intern/Base64VLQ.h"

#include "SourceMap/RevisionThree.h"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace SourceMap::Extension::intern {

namespace {

using namespace std::string_view_literals;

auto const SEGMENT_DELIMITER = ',';
auto const ENTRY_DELIMITER = ';';
auto const CALLSTACK_FORMAT_KEY = "x_de_hicknhack_software_callstack"sv;
auto const CALLERS_KEY = "callers"sv;
auto const CALLER_INDICES_KEY = "indices"sv;

auto encodeCallerIndices(GeneratedLineCallerIndexList const& callerIndices) -> std::string {
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    auto encoded = std::string{};
    auto lastIndex = 0;
    auto lastLine = 1;
    auto newLine = true;
    for (auto const& p : callerIndices) {
        auto const line = std::get<0>(p);
        auto const index = std::get<1>(p);

        auto const lineDiff = (line - lastLine);
        for (auto i = 0; i < lineDiff; ++i) {
            encoded.push_back(ENTRY_DELIMITER);
        }

        if (lineDiff > 0) {
            lastLine = line;
            newLine = true;
        }

        if (lineDiff == 0 && newLine == false) {
            encoded.push_back(SEGMENT_DELIMITER);
        }

        if (index.value != -1) {
            Base64VLQ::encode(encoded, index.value - lastIndex);
            lastIndex = index.value;
        }

        newLine = false;
    }
    return encoded;
}

auto encodeCallerList(CallerList const& callers, std::vector<std::filesystem::path> const& sources) -> std::string {
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    auto encoded = std::string{};
    auto sourceIndex = 0;
    auto sourceLine = 1;
    auto sourceColumn = 1;
    auto parentIndex = 0;
    auto store = [&](int current, int& previous) {
        Base64VLQ::encode(std::ref(encoded), current - previous);
        previous = current;
    };

    for (SourceMap::Caller const& caller : callers) {
        if (encoded.size() > 0) {
            encoded.push_back(ENTRY_DELIMITER);
        }

        auto it = std::find(sources.begin(), sources.end(), caller.original.name);
        auto callerSourceIndex = -1;
        if (it != sources.end()) {
            callerSourceIndex = std::distance(sources.begin(), it);
        }

        store(callerSourceIndex, sourceIndex);
        store(caller.original.position.line, sourceLine);
        store(caller.original.position.column, sourceColumn);
        if (InvalidCallerIndex != caller.parentIndex.value) {
            store(caller.parentIndex.value, parentIndex);
        }
    }
    return encoded;
}

} // namespace

auto buildCallerStack(CallerList const& callers, CallerIndex index) -> CallerStack {
    CallerStack stack;
    while (InvalidCallerIndex != index.value) {
        stack.push_back(callers[index.value].original);
        index = callers[index.value].parentIndex;
    }
    return stack;
}

void jsonStoreCallers(
    std::reference_wrapper<RevisionThree> json,
    CallerList const& callers,
    GeneratedLineCallerIndexList const& callerIndices) {
    auto callerObject = nlohmann::json::object();
    auto const sources = json.get().sources();
    auto const encodedCallers = encodeCallerList(callers, sources);
    callerObject[CALLERS_KEY] = encodedCallers;

    auto const encodedIndices = encodeCallerIndices(callerIndices);
    callerObject[CALLER_INDICES_KEY] = encodedIndices;

    json.get().json()[CALLSTACK_FORMAT_KEY] = callerObject;
}

auto jsonDecodeCallerIndices(RevisionThree const& json) -> CallerIndexList {
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    auto result = CallerIndexList{};
    if (!json.json().contains(CALLSTACK_FORMAT_KEY)) return result;

    auto callerObject = json.json()[CALLSTACK_FORMAT_KEY];
    if (!callerObject.contains(CALLER_INDICES_KEY)) return result;

    auto const encoded = callerObject[CALLER_INDICES_KEY].get<std::string>();
    auto mappingLines = std::string_view{json.json().at("mappings").get_ref<std::string const&>()};
    auto indexLines = std::string_view{encoded};
    auto lastIndex = 0;
    auto take = [](std::string_view& remaining, char delimiter) {
        auto separator = remaining.find(delimiter);
        auto part = remaining.substr(0, separator);
        remaining = separator == remaining.npos ? std::string_view{} : remaining.substr(separator + 1);
        return part;
    };
    // Empty extension segments mean "no caller", but empty mapping lines have
    // no entries at all. Use the base mappings to distinguish the two cases.
    while (!mappingLines.empty()) {
        auto mappingLine = take(mappingLines, ENTRY_DELIMITER);
        auto indexLine = take(indexLines, ENTRY_DELIMITER);
        while (!mappingLine.empty()) {
            take(mappingLine, SEGMENT_DELIMITER);
            auto segment = std::string{take(indexLine, SEGMENT_DELIMITER)};
            auto begin = segment.cbegin();
            auto index = InvalidCallerIndex;
            if (!segment.empty() && Base64VLQ::decode(begin, segment.cend(), std::ref(index)))
                lastIndex = (index += lastIndex);
            result.push_back(CallerIndex{index});
        }
    }
    return result;
}

auto jsonDecodeCallerList(RevisionThree const& json) -> CallerList {
    namespace Base64VLQ = SourceMap::intern::Base64VLQ;

    auto result = CallerList{};
    if (!json.json().contains(CALLSTACK_FORMAT_KEY)) return result;

    auto const callerObject = json.json()[CALLSTACK_FORMAT_KEY];
    if (!callerObject.contains(CALLERS_KEY)) return result;

    auto const sourceNameAt = [sources = json.sources()](int i) -> std::filesystem::path {
        return i >= 0 && static_cast<size_t>(i) < sources.size() ? sources[static_cast<size_t>(i)]
                                                                 : std::filesystem::path{};
    };

    auto const encodedCallers = callerObject[CALLERS_KEY].get<std::string>();
    auto begin = encodedCallers.begin();
    auto const end = encodedCallers.end();
    auto const load = [&begin, end](int& reference, int value) {
        if (Base64VLQ::decode(begin, end, std::ref(value))) reference = (value += reference);
        return value;
    };

    auto sourceIndex = 0;
    auto sourcePosition = Position{.line = 1, .column = 1};
    // Match encodeCallerList's delta accumulator. An omitted parent remains -1,
    // but the initial reference for an encoded parent index is zero.
    auto parentIndex = CallerIndex{0};
    while (begin != end) {
        if (*begin == ENTRY_DELIMITER) {
            begin++;
            continue;
        }
        auto const callerSourceIndex = load(sourceIndex, -1);
        auto const callerSourceLine = load(sourcePosition.line, 0);
        auto const callerSourceColumn = load(sourcePosition.column, 0);
        auto const callerParentIndex = load(parentIndex.value, InvalidCallerIndex);
        result.emplace_back(
            SourceMap::Caller{
                .original =
                    FilePosition{
                        .name = sourceNameAt(callerSourceIndex),
                        .position = Position{.line = callerSourceLine, .column = callerSourceColumn},
                    },
                .parentIndex = CallerIndex{.value = callerParentIndex},
            });
    }
    return result;
}

} // namespace SourceMap::Extension::intern
