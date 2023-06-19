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
#include "tconf/detail/config_reader_ptr.h"
#include "tconf/detail/interface.h"
#include "tconf/tree/tree.h"
#include <memory>
#include <string>

namespace tconf::detail {
class INode;
class IParam;
class IValidator;

class IConfigReader : private sfun::interface<IConfigReader> {
public:
    virtual ConfigReaderPtr makeNestedReader(const std::string& name) = 0;
    virtual void load(const tconf::TreeNode& node) = 0;
    virtual void addNode(const std::string& name, std::unique_ptr<INode> cfg) = 0;
    virtual void addParam(const std::string& name, std::unique_ptr<IParam> param) = 0;
    virtual void addValidator(std::unique_ptr<IValidator> validator) = 0;

protected:
    ConfigReaderPtr makePtr()
    {
        return this;
    }

    template<typename TCfg>
    void resetConfigReader(TCfg& cfg)
    {
        cfg.cfgReader_ = ConfigReaderPtr{};
    }
};

} //namespace tconf::detail