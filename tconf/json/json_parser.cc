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

#include "json_parser.h"
#include "tconf/errors.h"
#include "jsoncons/json.hpp"
#include <regex>

namespace tconf {

    namespace {
        void parseJson(const jsoncons::json &json, tconf::TreeNode &node) {
            for (auto &it: json.object_range()) {
                if (it.value().is_object()) {
                    auto &newNode = node.asItem().addNode(it.key());
                    parseJson(it.value(), newNode);
                } else if (it.value().is_array()) {
                    if (!it.value().empty() && it.value().array_range().begin()->is_object()) {
                        auto &newNode = node.asItem().addNodeList(it.key());
                        for (auto &item: it.value().array_range())
                            parseJson(item, newNode.asList().addNode());
                    } else {
                        auto valuesList = std::vector<std::string>{};
                        for (auto &item: it.value().array_range())
                            valuesList.emplace_back(item.as_string());

                        node.asItem().addParamList(it.key(), valuesList);
                    }
                } else {
                    node.asItem().addParam(it.key(), it.value().as_string());
                }
            }
        }

        ConfigError makeConfigError(const std::exception &e) {
            auto message = std::string{e.what()};
            auto regex = std::regex{R"(.*\line (\d+), column (\d+): (.*))"};
            auto match = std::smatch{};
            if (std::regex_search(message, match, regex)) {
                auto line = std::stoi(match[1]);
                auto column = std::stoi(match[2]);
                auto error = match[3].str();
                if (!error.empty())
                    error[0] = static_cast<char>(std::toupper(error[0]));

                return {error, tconf::StreamPosition{line, column}};
            }
            return {e.what()};
        }
    } //namespace

    TreeNode JsonParser::parse(std::istream &stream) {
        auto json = jsoncons::json{};
        try {
            stream >> json;
        }
        catch (const std::exception &e) {
            throw makeConfigError(e);
        }

        auto tree = tconf::makeTreeRoot();
        parseJson(json, tree);
        return tree;
    }

} // namespace tconf