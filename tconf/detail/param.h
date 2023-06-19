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
#include "tconf/detail/iconfig_entity.h"
#include "tconf/detail/iparam.h"
#include "tconf/detail/string_converter.h"
#include "tconf/detail/utils.h"
#include "tconf/detail/functional.h"
#include "tconf/detail/type_traits.h"
#include <tconf/errors.h>
#include <tconf/tree/string_converter.h>
#include <tconf/tree/tree.h>
#include <algorithm>
#include <sstream>
#include <string>

namespace tconf::detail {

template<typename T>
class Param : public IParam {
public:
    Param(std::string name, T& paramValue)
        : name_{std::move(name)}
        , paramValue_{paramValue}
    {
    }

    void markValueIsSet()
    {
        hasValue_ = true;
    }

private:
    void load(const TreeParam& param) override
    {
        hasValue_ = true;
        position_ = param.position();
        if (!param.isItem())
            throw ConfigError{"Parameter '" + name_ + "': config parameter can't be a list.", param.position()};
        auto paramReadResult = convertFromString<T>(param.value());
        auto readResultVisitor = sfun::overloaded{
                [&](const T& param)
                {
                    paramValue_ = param;
                },
                [&](const StringConversionError& error)
                {
                    throw ConfigError{
                            "Couldn't set parameter '" + name_ + "' value from '" + param.value() + "'" +
                                    (!error.message.empty() ? ": " + error.message : ""),
                            param.position()};
                }};

        std::visit(readResultVisitor, paramReadResult);
    }

    bool hasValue() const override
    {
        if constexpr (sfun::is_optional_v<T>)
            return true;
        else
            return hasValue_;
    }

    StreamPosition position() override
    {
        return position_;
    }

    std::string description() override
    {
        return "Parameter '" + name_ + "'";
    }

private:
    std::string name_;
    T& paramValue_;
    bool hasValue_ = false;
    StreamPosition position_;
};

} //namespace tconf::detail