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
#include "tconf/detail/node.h"
#include "tconf/detail/utils.h"
#include "tconf/detail/validator.h"
#include "tconf/detail/contract.h"
#include "tconf/detail/type_traits.h"
#include "tconf/detail/utility.h"
#include <tconf/config.h>
#include <tconf/name_format.h>
#include <type_traits>

namespace tconf {
class Config;
}

namespace tconf::detail {

template<typename TCfg>
class NodeCreator {
    static_assert(
            std::is_base_of_v<Config, sfun::remove_optional_t<TCfg>>,
            "TConfig must be a subclass of tconf::Config.");

public:
    NodeCreator(ConfigReaderPtr cfgReader, std::string nodeName, TCfg& nodeCfg)
        : cfgReader_{cfgReader}
        , nodeName_{(sfun_precondition(!nodeName.empty()), std::move(nodeName))}
        , nodeCfg_{nodeCfg}
        , nestedCfgReader_{cfgReader_ ? cfgReader_->makeNestedReader(nodeName_) : ConfigReaderPtr{}}
    {
        if constexpr (sfun::is_optional_v<TCfg>)
            static_assert(
                    sfun::dependent_false<TCfg>,
                    "TConfig can't be placed in std::optional, use tconf::optional instead.");
        node_ = std::make_unique<Node<TCfg>>(nodeName_, nodeCfg_, nestedCfgReader_);
    }

    NodeCreator<TCfg>& operator()()
    {
        node_->markValueIsSet();
        return *this;
    }

    operator TCfg()
    {
        if (cfgReader_)
            cfgReader_->addNode(nodeName_, std::move(node_));

        if constexpr (!std::is_aggregate_v<TCfg>)
            static_assert(
                    std::is_constructible_v<TCfg, detail::ConfigReaderPtr>,
                    "Non aggregate config objects must inherit tconf::Config constructors with 'using "
                    "Config::Config;'");

        return TCfg{nestedCfgReader_};
    }

    NodeCreator<TCfg>& ensure(std::function<void(const TCfg&)> validatingFunc)
    {
        if (cfgReader_)
            cfgReader_->addValidator(std::make_unique<Validator<TCfg>>(*node_, nodeCfg_, std::move(validatingFunc)));
        return *this;
    }

    template<typename TValidator, typename... TArgs>
    NodeCreator<TCfg>& ensure(TArgs&&... args)
    {
        if (cfgReader_)
            cfgReader_->addValidator(
                    std::make_unique<Validator<TCfg>>(*node_, nodeCfg_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    ConfigReaderPtr cfgReader_;
    std::string nodeName_;
    TCfg& nodeCfg_;
    ConfigReaderPtr nestedCfgReader_;
    std::unique_ptr<Node<TCfg>> node_;
};

} //namespace tconf::detail