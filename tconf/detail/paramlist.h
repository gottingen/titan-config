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
#include "tconf/detail/iparam.h"
#include "tconf/detail/string_converter.h"
#include "tconf/detail/utils.h"
#include "tconf/detail/type_traits.h"
#include <tconf/errors.h>
#include <tconf/tree/string_converter.h>
#include <tconf/tree/tree.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace tconf::detail {

template<typename TParamList>
class ParamList : public IParam {
    static_assert(
            sfun::is_dynamic_sequence_container_v<sfun::remove_optional_t<TParamList>>,
            "Param list field must be a sequence container or a sequence container placed in std::optional");

public:
    ParamList(std::string name, TParamList& paramValue)
        : name_{std::move(name)}
        , paramListValue_{paramValue}
    {
    }

    void markValueIsSet()
    {
        hasValue_ = true;
    }

private:
    void load(const TreeParam& paramList) override
    {
        position_ = paramList.position();
        hasValue_ = true;
        if constexpr (sfun::is_optional_v<TParamList>)
            paramListValue_.emplace();

        if (!paramList.isList())
            throw ConfigError{"Parameter list '" + name_ + "': config parameter must be a list.", paramList.position()};
        for (const auto& paramValueStr : paramList.valueList()) {
            using Param = typename sfun::remove_optional_t<TParamList>::value_type;
            auto paramReadResult = convertFromString<Param>(paramValueStr);
            auto readResultVisitor = sfun::overloaded{
                    [&](const Param& param)
                    {
                        maybeOptValue(paramListValue_).emplace_back(param);
                    },
                    [&](const StringConversionError& error)
                    {
                        throw ConfigError{
                                "Couldn't set parameter list element'" + name_ + "' value from '" + paramValueStr +
                                        "'" + (!error.message.empty() ? ": " + error.message : ""),
                                paramList.position()};
                    }};

            std::visit(readResultVisitor, paramReadResult);
        }
    }

    bool hasValue() const override
    {
        if constexpr (sfun::is_optional_v<TParamList>)
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
        return "Parameter list '" + name_ + "'";
    }

private:
    std::string name_;
    TParamList& paramListValue_;
    bool hasValue_ = false;
    StreamPosition position_;
};

} //namespace tconf::detail