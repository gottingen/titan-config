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

#include "tconf/detail/inode.h"
#include "tconf/detail/loading_error.h"
#include "tconf/detail/utils.h"
#include "tconf/detail/type_traits.h"
#include <tconf/errors.h>
#include <tconf/tree/tree.h>
#include <memory>
#include <type_traits>
#include <vector>

namespace tconf::detail {

    enum class NodeListType {
        Normal,
        Copy
    };

    template<typename TCfgList>
    class NodeList : public detail::INode {
    public:
        NodeList(std::string name, TCfgList &nodeList, ConfigReaderPtr cfgReader,
                 NodeListType type = NodeListType::Normal)
                : name_{std::move(name)}, nodeList_{nodeList}, type_{type}, cfgReader_{cfgReader} {
            static_assert(
                    sfun::is_dynamic_sequence_container_v<sfun::remove_optional_t<TCfgList>>,
                    "Node list field must be a sequence container or a sequence container placed in std::optional");
        }

        void markValueIsSet() {
            hasValue_ = true;
        }

        void load(const TreeNode &nodeList) override {
            hasValue_ = true;
            position_ = nodeList.position();
            if (!nodeList.isList())
                throw ConfigError{"Node list '" + name_ + "': config node must be a list.", nodeList.position()};
            if constexpr (sfun::is_optional<TCfgList>::value)
                nodeList_.emplace();

            maybeOptValue(nodeList_).clear();
            for (auto i = 0; i < nodeList.asList().count(); ++i) {
                const auto &treeNode = nodeList.asList().node(i);
                try {
                    using Cfg = typename sfun::remove_optional_t<TCfgList>::value_type;
                    if constexpr (!std::is_aggregate_v<Cfg>)
                        static_assert(
                                std::is_constructible_v<Cfg, detail::ConfigReaderPtr>,
                                "Non aggregate config objects must inherit tconf::Config constructors with 'using "
                                "Config::Config;'");

                    auto cfg = Cfg{cfgReader_};
                    if (cfgReader_) {
                        if (type_ == NodeListType::Copy && i > 0)
                            cfgReader_->load(nodeList.asList().node(0));
                        cfgReader_->load(treeNode);
                    }
                    maybeOptValue(nodeList_).emplace_back(std::move(cfg));
                }
                catch (const LoadingError &e) {
                    throw ConfigError{"Node list '" + name_ + "': " + e.what(), treeNode.position()};
                }
            }
        }

        bool hasValue() const override {
            if constexpr (sfun::is_optional_v<TCfgList>)
                return true;
            else
                return hasValue_;
        }

        StreamPosition position() override {
            return position_;
        }

        std::string description() override {
            return "Node list '" + name_ + "'";
        }

    private:
        std::string name_;
        TCfgList &nodeList_;
        bool hasValue_ = false;
        StreamPosition position_;
        NodeListType type_;
        ConfigReaderPtr cfgReader_;
    };

} //namespace tconf::detail