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
#include "tconf/yaml/parser.h"
#include "tconf/yaml/rapidyaml.h"
#include "tconf/errors.h"
#include "tconf/tree/iparser.h"
#include <iterator>
#include <regex>
#include <string>
#include <vector>

namespace tconf::yaml::detail {
    namespace {
        void on_error(const char *msg, size_t len, ryml::Location loc, void *) {
            throw tconf::ConfigError{std::string{msg, len}, {loc.line, loc.col}};
        }

        ryml::Callbacks errorCallback() {
            return {nullptr, nullptr, nullptr, on_error};
        }

        void parseYaml(const ryml::ConstNodeRef &yaml, tconf::TreeNode &node) {
            auto str = [](const auto &yamlstr) {
                return std::string{yamlstr.data(), yamlstr.size()};
            };
            if (yaml.is_stream()) {
                parseYaml(yaml[0], node);
                return;
            }
            for (const auto &child: yaml.children()) {
                if (child.is_map()) {
                    auto &newNode = node.asItem().addNode(str(child.key()));
                    parseYaml(child, newNode);
                } else if (child.is_container()) {
                    if (child.has_children() && child.first_child().is_map()) {
                        auto &newNode = node.asItem().addNodeList(str(child.key()));
                        for (const auto &item: child.children())
                            parseYaml(item, newNode.asList().addNode());
                    } else {
                        auto valuesList = std::vector<std::string>{};
                        for (auto item: child.children())
                            valuesList.emplace_back(str(item.val()));

                        node.asItem().addParamList(str(child.key()), valuesList);
                    }
                } else if (child.is_keyval())
                    node.asItem().addParam(str(child.key()), str(child.val()));
            }
        }
    } //namespace
} //namespace tconf::yaml::detail

namespace tconf::yaml {
    TreeNode Parser::parse(std::istream &stream) {
        stream >> std::noskipws;

        auto input = std::string{std::istream_iterator<char>{stream}, std::istream_iterator<char>{}};
        auto yaml = ryml::Tree{};
        ryml::set_callbacks(detail::errorCallback());
        yaml = ryml::parse_in_arena(ryml::to_csubstr(input));

        auto tree = tconf::makeTreeRoot();
        detail::parseYaml(yaml.rootref(), tree);

        return tree;
    }

} //namespace tconf::yaml