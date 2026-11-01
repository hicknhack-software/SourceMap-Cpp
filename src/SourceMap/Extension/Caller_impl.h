// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

#include "SourceMap/Extension/Caller.h"

#include "SourceMap/Data.h"
#include "SourceMap/Entry.h"

namespace SourceMap {
namespace Extension {
namespace intern {

auto buildCallerStack(CallerList const& callers, CallerIndex index) -> CallerStack;

using CallerIndexList = std::vector<CallerIndex>;
using GeneratedLineCallerIndexList = std::vector<std::tuple<int, CallerIndex>>;

auto jsonDecodeCallerIndices(RevisionThree const& json) -> CallerIndexList;
auto jsonDecodeCallerList(RevisionThree const& json) -> CallerList;
void jsonStoreCallers(
    std::reference_wrapper<RevisionThree> json,
    CallerList const& callers,
    GeneratedLineCallerIndexList const& callerIndices);

template<typename Data>
auto extractCallerIndices(Data const& data) -> GeneratedLineCallerIndexList {
    auto result = GeneratedLineCallerIndexList{};
    auto const& entries = data.entries;
    result.reserve(entries.size());
    for (auto const& entry : entries) {
        auto const callerIndex = SourceMap::get<SourceMap::Extension::Caller>(entry);
        result.push_back(std::make_tuple(entry.generated.line, callerIndex));
    }
    return result;
}

template<typename Data>
auto extractCallerList(Data const& data) -> SourceMap::CallerList {
    return SourceMap::get<SourceMap::Extension::Caller>(data);
}

template<typename Data>
void injectCallerIndices(std::reference_wrapper<Data> data, intern::CallerIndexList&& list) {
    auto begin = list.begin();
    auto end = list.end();
    for (auto& entry : data.get().entries) {
        if (begin == end) break;
        SourceMap::get<SourceMap::Extension::Caller>(entry) = *begin++;
    }
}

template<typename Data>
void injectCallerList(std::reference_wrapper<Data> data, SourceMap::CallerList&& list) {
    SourceMap::get<SourceMap::Extension::Caller>(data.get()) = std::move(list);
}

template<typename Callback>
void collectFileNames(Callback&& cb, SourceMap::CallerList const& list) {
    for (auto const& caller : list) cb(caller.original.name);
}

} // namespace intern

template<typename Data>
void Caller::jsonEncode(Data const& data, std::reference_wrapper<RevisionThree> json) {
    intern::jsonStoreCallers(json, intern::extractCallerList(data), intern::extractCallerIndices(data));
}

template<typename Data>
auto Caller::jsonDecode(std::reference_wrapper<Data> data, RevisionThree const& json) -> bool {
    intern::injectCallerIndices(data, intern::jsonDecodeCallerIndices(json));
    intern::injectCallerList(data, intern::jsonDecodeCallerList(json));
    return true;
}

template<typename Data, typename Callback>
void Caller::collectFileNames(Data const& data, Callback&& cb) {
    intern::collectFileNames((Callback&&)cb, intern::extractCallerList(data));
}

} // namespace Extension

template<typename... ExtensionTypes>
auto buildCallerStack(Data<ExtensionTypes...> const& data, Entry<ExtensionTypes...> const* entry) -> CallerStack {
    if (nullptr == entry) return CallerStack{};
    return Extension::intern::buildCallerStack(get<Extension::Caller>(data), get<Extension::Caller>(*entry));
}

template<typename... ExtensionTypes>
auto buildCallerStack(Mapping<ExtensionTypes...> const& mapping, Entry<ExtensionTypes...> const* entry) -> CallerStack {
    return buildCallerStack(mapping.data(), entry);
}

} // namespace SourceMap
