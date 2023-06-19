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

#ifndef TCONF_TREE_TREE_H_
#define TCONF_TREE_TREE_H_

#include "tconf/errors.h"
#include "tconf/tree/stream_position.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace tconf {

    class TreeParam {
        struct Item {
            std::string value;
        };
        struct List {
            std::vector<std::string> valueList;
        };

    public:
        TreeParam(std::string value, const StreamPosition &position = {})
                : position_(position), data_(Item{std::move(value)}) {
        }

        TreeParam(std::vector<std::string> valueList, const StreamPosition &position = {})
                : position_(position), data_(List{std::move(valueList)}) {
        }

        bool isItem() const {
            return std::holds_alternative<Item>(data_);
        }

        bool isList() const {
            return std::holds_alternative<List>(data_);
        }

        const std::string &value() const {
            return std::get<Item>(data_).value;
        }

        const std::vector<std::string> valueList() const {
            return std::get<List>(data_).valueList;
        }

        StreamPosition position() const {
            return position_;
        }

    private:
        StreamPosition position_;
        std::variant<Item, List> data_;
    };

    class TreeNode {
    public:
        class List {
        public:
            int count() const {
                return static_cast<int>(nodeList_.size());
            }

            const TreeNode &node(int index) const {
                return *nodeList_.at(static_cast<std::size_t>(index));
            }

            TreeNode &addNode(const StreamPosition &pos = {}) {
                auto node = std::make_unique<TreeNode>();
                node->position_ = pos;
                node->data_.emplace<TreeNode::Item>();
                return *nodeList_.emplace_back(std::move(node));
            }

        private:
            std::vector<std::unique_ptr<TreeNode>> nodeList_;
        };

        class Item {
        public:
            int paramsCount() const {
                return static_cast<int>(params_.size());
            }

            int nodesCount() const {
                return static_cast<int>(nodes_.size());
            }

            bool hasParam(const std::string &name) const {
                return params_.find(name) != params_.end();
            }

            bool hasNode(const std::string &name) const {
                return nodes_.find(name) != nodes_.end();
            }

            const TreeParam &param(const std::string &name) const {
                return params_.at(name);
            }

            const std::map<std::string, TreeParam> &params() const {
                return params_;
            }

            const TreeNode &node(const std::string &name) const {
                return nodes_.at(name);
            }

            const std::map<std::string, TreeNode> &nodes() const {
                return nodes_;
            }

            TreeNode &addNode(const std::string &name, const StreamPosition &pos = {}) {
                auto node = TreeNode{};
                node.position_ = pos;
                node.data_.emplace<Item>();
                auto [it, ok] = nodes_.emplace(name, std::move(node));
                if (!ok)
                    throw ConfigError{"Node '" + name + "' already exists", pos};

                return it->second;
            }

            TreeNode &addNodeList(const std::string &name, const StreamPosition &pos = {}) {
                auto node = TreeNode{};
                node.position_ = pos;
                node.data_.emplace<List>();
                auto [it, ok] = nodes_.emplace(name, std::move(node));
                if (!ok)
                    throw ConfigError{"Node list '" + name + "' already exists", pos};

                return it->second;
            }

            void addParam(const std::string &name, const std::string &value, const StreamPosition &pos = {}) {
                auto [_, ok] = params_.emplace(name, TreeParam{value, pos});
                if (!ok)
                    throw ConfigError{"Parameter '" + name + "' already exists", pos};
            }

            void addParamList(
                    const std::string &name,
                    const std::vector<std::string> &valueList,
                    const StreamPosition &pos = {}) {
                auto [_, ok] = params_.emplace(name, TreeParam{valueList, pos});
                if (!ok)
                    throw ConfigError{"Parameter list '" + name + "' already exists", pos};
            }

        private:
            std::map<std::string, TreeParam> params_;
            std::map<std::string, TreeNode> nodes_;
        };

        bool isItem() const {
            return std::holds_alternative<Item>(data_);
        }

        bool isList() const {
            return std::holds_alternative<List>(data_);
        }

        const Item &asItem() const {
            return std::get<Item>(data_);
        }

        const List &asList() const {
            return std::get<List>(data_);
        }

        Item &asItem() {
            return std::get<Item>(data_);
        }

        List &asList() {
            return std::get<List>(data_);
        }

        const StreamPosition &position() const {
            return position_;
        }

        bool isRoot() const {
            return isRoot_;
        }

    private:
        std::variant<Item, List> data_;
        bool isRoot_ = false;
        StreamPosition position_{1, 1};

        friend TreeNode makeTreeRoot();
    };

    inline TreeNode makeTreeRoot() {
        auto root = TreeNode{};
        root.isRoot_ = true;
        return root;
    }

} //namespace tconf

#endif  // TCONF_TREE_TREE_H_
