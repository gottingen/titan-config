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

#include "tconf/detail/string_utils.h"
#include <algorithm>
#include <string>

namespace tconf::detail {

    inline std::string formatName(const std::string &name) {
        auto result = name;
        //remove front non-alphabet characters
        result.erase(
                result.begin(),
                std::find_if(
                        result.begin(),
                        result.end(),
                        [](auto ch) {
                            return sfun::isalpha(ch);
                        }));
        //remove back non-alphabet and non-digit characters
        result.erase(
                std::find_if(
                        result.rbegin(),
                        result.rend(),
                        [](auto ch) {
                            return sfun::isalnum(ch);
                        })
                        .base(),
                result.end());
        return result;
    }

    inline std::string toCamelCase(const std::string &name) {
        auto result = std::string{};
        auto prevCharNonAlpha = false;
        auto formattedName = formatName(name);
        if (!formattedName.empty())
            formattedName[0] = static_cast<char>(sfun::tolower(formattedName[0]));
        for (auto ch: formattedName) {
            if (!sfun::isalpha(ch)) {
                if (sfun::isdigit(ch))
                    result.push_back(ch);
                if (!result.empty())
                    prevCharNonAlpha = true;
                continue;
            }
            if (prevCharNonAlpha)
                ch = static_cast<char>(sfun::toupper(ch));
            result.push_back(ch);
            prevCharNonAlpha = false;
        }
        return result;
    }

    inline std::string toKebabCase(const std::string &name) {
        auto result = std::string{};
        auto formattedName = formatName(sfun::replace(name, "_", "-"));
        if (!formattedName.empty())
            formattedName[0] = sfun::tolower(formattedName[0]);
        for (auto ch: formattedName) {
            if (sfun::isupper(ch) && !result.empty()) {
                result.push_back('-');
                result.push_back(sfun::tolower(ch));
            } else
                result.push_back(ch);
        }
        return result;
    }

    inline std::string toSnakeCase(const std::string &name) {
        auto result = std::string{};
        auto formattedName = formatName(name);
        if (!formattedName.empty())
            formattedName[0] = sfun::tolower(formattedName[0]);
        for (auto ch: formattedName) {
            if (sfun::isupper(ch) && !result.empty()) {
                result.push_back('_');
                result.push_back(sfun::tolower(ch));
            } else
                result.push_back(ch);
        }
        return result;
    }

} //namespace tconf::detail
