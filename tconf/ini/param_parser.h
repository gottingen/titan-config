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

#include <optional>
#include <string>
#include <vector>

namespace tconf::ini::detail {
    class Stream;

    std::optional<std::string> readParam(
            Stream &stream,
            const std::string &wordSeparator,
            const std::vector<std::string> &paramListValue,
            const std::string &paramName);

    std::optional<std::vector<std::string>> readParamList(const std::string &paramName, const std::string &paramValue);

    std::string readParam(const std::string &paramValue);

} //namespace tconf::ini::detail