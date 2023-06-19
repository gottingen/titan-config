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
#include "tconf/detail/iconfig_reader.h"
#include "tconf/detail/param.h"
#include "tconf/detail/validator.h"
#include "tconf/detail/contract.h"

namespace tconf::detail {

template<typename T>
class ParamCreator {
public:
    ParamCreator(ConfigReaderPtr cfgReader, std::string paramName, T& paramValue)
        : cfgReader_{cfgReader}
        , paramName_{(sfun_precondition(!paramName.empty()), std::move(paramName))}
        , paramValue_{paramValue}
        , param_{std::make_unique<Param<T>>(paramName_, paramValue)}
    {
    }

    ParamCreator<T>& operator()(T defaultValue = {})
    {
        defaultValue_ = std::move(defaultValue);
        param_->markValueIsSet();
        return *this;
    }

    ParamCreator<T>& ensure(std::function<void(const T&)> validatingFunc)
    {
        if (cfgReader_)
            cfgReader_->addValidator(std::make_unique<Validator<T>>(*param_, paramValue_, std::move(validatingFunc)));
        return *this;
    }

    template<typename TValidator, typename... TArgs>
    ParamCreator<T>& ensure(TArgs&&... args)
    {
        if (cfgReader_)
            cfgReader_->addValidator(
                    std::make_unique<Validator<T>>(*param_, paramValue_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

    operator T()
    {
        if (cfgReader_)
            cfgReader_->addParam(paramName_, std::move(param_));
        return defaultValue_;
    }

private:
    ConfigReaderPtr cfgReader_;
    std::string paramName_;
    T& paramValue_;
    std::unique_ptr<Param<T>> param_;
    T defaultValue_;
};

} //namespace tconf::detail