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
#include "tconf/detail/inode.h"
#include "tconf/detail/paramlist.h"
#include "tconf/detail/utils.h"
#include "tconf/detail/validator.h"
#include "tconf/detail/contract.h"
#include "tconf/detail/type_traits.h"
#include <vector>

namespace tconf::detail {

template<typename TParamList>
class ParamListCreator {
    static_assert(
            sfun::is_dynamic_sequence_container_v<sfun::remove_optional_t<TParamList>>,
            "Param list field must be a sequence container or a sequence container placed in std::optional");

public:
    ParamListCreator(ConfigReaderPtr cfgReader, std::string paramListName, TParamList& paramListValue)
        : cfgReader_{cfgReader}
        , paramListName_{(sfun_precondition(!paramListName.empty()), std::move(paramListName))}
        , paramListValue_{paramListValue}
        , paramList_{std::make_unique<ParamList<TParamList>>(paramListName_, paramListValue)}
    {
    }

    ParamListCreator<TParamList>& operator()(TParamList defaultValue = {})
    {
        defaultValue_ = std::move(defaultValue);
        paramList_->markValueIsSet();
        return *this;
    }

    ParamListCreator<TParamList>& ensure(std::function<void(const TParamList&)> validatingFunc)
    {
        if (cfgReader_)
            cfgReader_->addValidator(
                    std::make_unique<Validator<TParamList>>(*paramList_, paramListValue_, std::move(validatingFunc)));
        return *this;
    }

    template<typename TValidator, typename... TArgs>
    ParamListCreator<TParamList>& ensure(TArgs&&... args)
    {
        if (cfgReader_)
            cfgReader_->addValidator(std::make_unique<Validator<TParamList>>(
                    *paramList_,
                    paramListValue_,
                    TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

    operator TParamList()
    {
        if (cfgReader_)
            cfgReader_->addParam(paramListName_, std::move(paramList_));
        return defaultValue_;
    }

private:
    ConfigReaderPtr cfgReader_;
    std::string paramListName_;
    TParamList& paramListValue_;
    std::unique_ptr<ParamList<TParamList>> paramList_;
    TParamList defaultValue_;
};

} //namespace tconf::detail