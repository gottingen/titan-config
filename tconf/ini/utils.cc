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
#include "utils.h"
#include "stream.h"
#include "tconf/errors.h"

namespace tconf::ini::detail {

    void skipWhitespace(Stream &stream, bool withNewLine) {
        while (!stream.atEnd()) {
            auto nextChar = stream.peek().front();
            if (!withNewLine && nextChar == '\n')
                return;

            if (std::isspace(nextChar))
                stream.skip(1);
            else
                return;
        }
    }

    std::string readUntil(Stream &stream, std::function<bool(char)> stopPred) {
        auto result = std::string{};
        while (!stream.atEnd()) {
            if (stopPred(stream.peek().front()))
                return result;
            result += stream.read();
        }
        return result;
    }

    std::string readWord(Stream &stream, const std::string &stopChars) {
        return readUntil(
                stream,
                [&stopChars](char ch) {
                    return std::isspace(ch) || stopChars.find(ch) != std::string::npos;
                });
    }

    std::optional<std::string> readQuotedString(Stream &stream) {
        if (stream.atEnd())
            return {};

        auto quotationMark = stream.peek().front();
        if (quotationMark != '\'' && quotationMark != '"')
            return {};
        auto pos = stream.position();
        stream.skip(1);

        auto result = std::string{};
        while (!stream.atEnd()) {
            auto ch = stream.read().front();
            if (ch == quotationMark)
                return result;
            result += ch;
        }
        throw ConfigError{"String isn't closed", pos};
    }

} //namespace tconf::ini::detail
