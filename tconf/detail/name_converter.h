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
#include "tconf/detail/nameutils.h"
#include <tconf/name_format.h>

namespace tconf::detail {

template<NameFormat>
struct NameConverter;

template<>
struct NameConverter<NameFormat::Original> {
    static std::string name(const std::string& configName)
    {
        return formatName(configName);
    }
};

template<>
struct NameConverter<NameFormat::SnakeCase> {
    static std::string name(const std::string& configName)
    {
        return toSnakeCase(configName);
    }
};

template<>
struct NameConverter<NameFormat::CamelCase> {
    static std::string name(const std::string& configName)
    {
        return toCamelCase(configName);
    }
};

template<>
struct NameConverter<NameFormat::KebabCase> {
    static std::string name(const std::string& configName)
    {
        return toKebabCase(configName);
    }
};

} //namespace tconf::detail