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

#include "tconf/ini/param_parser.h"
#include "tconf/ini/stream.h"
#include "tconf/ini/utils.h"
#include "tconf/errors.h"
#include "tconf/tree/tree.h"
#include <sstream>
#include <utility>

namespace tconf::ini::detail {

    std::optional<std::string> readParam(
            Stream &stream,
            const std::string &wordSeparator,
            const std::vector<std::string> &paramListValue,
            const std::string &paramName) {
        auto quotedParam = readQuotedString(stream);
        if (quotedParam)
            return *quotedParam;
        else {
            auto result = readWord(stream, wordSeparator);
            if (result.empty()) {
                if (stream.peek() == ",")
                    throw ConfigError{"Parameter list '" + paramName + "' element is missing"};
                if (paramListValue.empty())
                    return std::nullopt;
            }
            return result;
        }
    }

    std::optional<std::vector<std::string>> readParamList(const std::string &paramName, const std::string &paramValue) {
        auto inputStream = std::stringstream{paramValue};
        auto stream = Stream{inputStream};
        skipWhitespace(stream, false);
        if (stream.peek() != "[")
            return std::nullopt;

        stream.skip(1);
        skipWhitespace(stream);
        auto paramValueList = std::vector<std::string>{};
        while (!stream.atEnd()) {
            auto param = readParam(stream, ",]", paramValueList, paramName);
            if (param)
                paramValueList.emplace_back(std::move(*param));

            skipWhitespace(stream, true);
            const auto endOfList = "]";
            if (stream.peek() == ",") {
                stream.skip(1);
                skipWhitespace(stream, true);
                if (stream.peek() == endOfList || stream.atEnd())
                    throw ConfigError{"Parameter list '" + paramName + "' element is missing"};
            } else if (stream.peek() == endOfList) {
                stream.skip(1);
                return paramValueList;
            }
        }
        throw ConfigError{"Wrong parameter list '" + paramName + "' format: missing ']' at the end"};
    }

    std::string readParam(const std::string &paramValue) {
        auto inputStream = std::stringstream{paramValue};
        auto stream = Stream{inputStream};
        skipWhitespace(stream, false);
        auto quotedParam = detail::readQuotedString(stream);
        if (quotedParam)
            return *quotedParam;
        else
            return paramValue;
    }

} //namespace tconf::ini::detail