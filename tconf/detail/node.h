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
#include "tconf/detail/iconfig_reader.h"
#include "tconf/detail/inode.h"
#include "tconf/detail/utils.h"
#include <tconf/errors.h>
#include <tconf/tree/tree.h>
#include <algorithm>
#include <sstream>
#include <string>

namespace tconf::detail {

template<typename TCfg>
class Node : public INode {
public:
    Node(std::string name, TCfg& cfg, ConfigReaderPtr cfgReader)
        : name_{std::move(name)}
        , cfg_{cfg}
        , cfgReader_{cfgReader}
    {
    }

    void markValueIsSet()
    {
        hasValue_ = true;
    }

private:
    void load(const TreeNode& node) override
    {
        hasValue_ = true;
        position_ = node.position();
        if (!node.isItem())
            throw ConfigError{"Node '" + name_ + "': config node can't be a list.", node.position()};

        if constexpr (is_initialized_optional_v<TCfg>)
            cfg_.emplace();

        if (cfgReader_)
            cfgReader_->load(node);
    }

    bool hasValue() const override
    {
        if constexpr (is_initialized_optional_v<TCfg>)
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
        return "Node '" + name_ + "'";
    }

private:
    std::string name_;
    TCfg& cfg_;
    ConfigReaderPtr cfgReader_;
    bool hasValue_ = false;
    StreamPosition position_;
};

} //namespace tconf::detail