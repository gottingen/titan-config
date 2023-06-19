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
#include "tconf/detail/dict.h"
#include "tconf/detail/iconfig_reader.h"
#include "tconf/detail/validator.h"
#include "tconf/detail/contract.h"
#include "tconf/detail/type_traits.h"
#include "tconf/name_format.h"
#include <memory>

namespace tconf::detail {

template<typename TMap>
class DictCreator {
public:
    DictCreator(ConfigReaderPtr cfgReader, std::string dictName, TMap& dictMap)
        : cfgReader_{cfgReader}
        , dictName_{(sfun_precondition(!dictName.empty()), std::move(dictName))}
        , dict_{std::make_unique<Dict<TMap>>(dictName_, dictMap)}
        , dictMap_{dictMap}
    {
        static_assert(
                sfun::is_associative_container_v<sfun::remove_optional_t<TMap>>,
                "Dictionary field must be an associative container or an associative container placed in "
                "std::optional");
        static_assert(
                std::is_same_v<typename sfun::remove_optional_t<TMap>::key_type, std::string>,
                "Dictionary associative container's key type must be std::string");
    }

    DictCreator& operator()(TMap defaultValue = {})
    {
        dict_->markValueIsSet();
        defaultValue_ = std::move(defaultValue);
        return *this;
    }

    operator TMap()
    {
        if (cfgReader_)
            cfgReader_->addNode(dictName_, std::move(dict_));
        return defaultValue_;
    }

    DictCreator& checkedWith(std::function<void(const TMap&)> validatingFunc)
    {
        if (cfgReader_)
            cfgReader_->addValidator(std::make_unique<Validator<TMap>>(*dict_, dictMap_, std::move(validatingFunc)));
        return *this;
    }

    template<typename TValidator, typename... TArgs>
    DictCreator& checkedWith(TArgs&&... args)
    {
        if (cfgReader_)
            cfgReader_->addValidator(
                    std::make_unique<Validator<TMap>>(*dict_, dictMap_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    ConfigReaderPtr cfgReader_;
    std::string dictName_;
    std::unique_ptr<Dict<TMap>> dict_;
    TMap& dictMap_;
    TMap defaultValue_;
};

} //namespace tconf::detail