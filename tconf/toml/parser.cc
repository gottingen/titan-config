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

#include <toml.hpp>
#include "tconf/toml/parser.h"
#include <tconf/errors.h>
#include <sstream>
#include <string>
#include <vector>

namespace tconf::toml {

    namespace {

        std::string str(const ::toml::value &val) {
            if (val.is_string())
                return val.as_string();

            auto stream = std::stringstream{};
            stream << val;
            return stream.str();
        }

        void parseToml(const ::toml::value &toml, TreeNode &node) {
            for (auto &[key, value]: toml.as_table()) {
                if (value.is_table()) {
                    auto &newNode = node.asItem().addNode(key);
                    parseToml(value, newNode);
                } else if (value.is_array()) {
                    if (!value.as_array().empty() && value.as_array().front().is_table()) {
                        auto &newNode = node.asItem().addNodeList(key);;
                        for (auto &item: value.as_array())
                            parseToml(item, newNode.asList().addNode());
                    } else {
                        auto valuesList = std::vector<std::string>{};
                        for (auto &item: value.as_array()) {
                            if (item.is_array())
                                throw tconf::ConfigError{
                                        "Array '" + key + "': toml doesn't support nested arrays"};
                            valuesList.emplace_back(str(item));
                        }

                        node.asItem().addParamList(key, valuesList);
                    }
                } else
                    node.asItem().addParam(key, str(value));
            }
        }
    } //namespace

    TreeNode Parser::parse(std::istream &stream) {
        const auto toml = [&] {
            try {
                return ::toml::parse(stream);
            }
            catch (const ::toml::exception &e) {
                throw tconf::ConfigError{e.what(), {e.location().line(), e.location().column()}};
            }
        }();

        auto tree = tconf::makeTreeRoot();
        parseToml(toml, tree);
        return tree;
    }

} //namespace tconf::toml