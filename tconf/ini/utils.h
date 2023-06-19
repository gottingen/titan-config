// Copyright 2023 The titan-search Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#pragma once

#include <functional>
#include <optional>
#include <string>

namespace tconf::ini::detail {
    class Stream;

    void skipWhitespace(Stream &stream, bool withNewLine = true);

    std::string readUntil(Stream &stream, std::function<bool(char)> stopPred);

    std::string readWord(Stream &stream, const std::string &stopChars = {});

    std::optional<std::string> readQuotedString(Stream &stream);

} //namespace tconf::ini::detail
