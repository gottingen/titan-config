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

#ifndef TCONF_CONFIG_READER_H_
#define TCONF_CONFIG_READER_H_

#include "tconf/config.h"
#include "tconf/errors.h"
#include "tconf/name_format.h"
#include "tconf/detail/path.h"
#include "tconf/detail/iconfig_reader.h"
#include "tconf/detail/inode.h"
#include "tconf/detail/iparam.h"
#include "tconf/detail/ivalidator.h"
#include "tconf/detail/loading_error.h"
#include "tconf/detail/name_converter.h"
#include "tconf/tree/iparser.h"
#include "tconf/tree/tree.h"
#include "tconf/json/json_parser.h"
#include "tconf/yaml/parser.h"
#include "tconf/ini/parser.h"
#include "tconf/toml/parser.h"
#include "turbo/files/filesystem.h"
#include <map>
#include <memory>
#include <type_traits>
#include <vector>

namespace tconf {

    void set_gflags_splitter(const std::string_view &spt);

    std::string get_gflags_splitter();

    template<NameFormat nameFormat = NameFormat::Original>
    class ConfigReader : public detail::IConfigReader {
    public:
        template<typename TCfg>
        TCfg read_file(const turbo::filesystem::path &configFile, IParser &parser) {
            if (!turbo::filesystem::exists(configFile))
                throw ConfigError{"Config file " + sfun::path_string(configFile) + " doesn't exist"};

            if (!turbo::filesystem::is_regular_file(configFile))
                throw ConfigError{
                        "Can't open config file " + sfun::path_string(configFile) + " which is not a regular file"};

            auto configStream = std::ifstream{configFile, std::ios_base::binary};
            if (!configStream.is_open())
                throw ConfigError{"Can't open config file " + sfun::path_string(configFile) + " for reading"};

            return read < TCfg > (configStream, parser);
        }

        template<typename TCfg>
        TCfg read(const std::string &configContent, IParser &parser) {
            auto configStream = std::stringstream{configContent};
            return read<TCfg>(configStream, parser);
        }

        template<typename TCfg>
        TCfg read_json_file(const turbo::filesystem::path &configFile) {
            auto parser = JsonParser{};
            return read_file<TCfg>(configFile, parser);
        }

        template<typename TCfg>
        TCfg read_json(const std::string &configContent) {
            auto configStream = std::stringstream{configContent};
            return read_json<TCfg>(configStream);
        }

        template<typename TCfg>
        TCfg read_json(std::istream &configStream) {
            auto parser = JsonParser{};
            return read<TCfg>(configStream, parser);
        }

        template<typename TCfg>
        TCfg read_yaml_file(const turbo::filesystem::path &configFile) {
            auto parser = yaml::Parser{};
            return read_file<TCfg>(configFile, parser);
        }

        template<typename TCfg>
        TCfg read_yaml(const std::string &configContent) {
            auto configStream = std::stringstream{configContent};
            return read_yaml<TCfg>(configStream);
        }

        template<typename TCfg>
        TCfg read_yaml(std::istream &configStream) {
            auto parser = yaml::Parser{};
            return read<TCfg>(configStream, parser);
        }

        template<typename TCfg>
        TCfg read_toml_file(const turbo::filesystem::path &configFile) {
            auto parser = toml::Parser{};
            return read_file<TCfg>(configFile, parser);
        }

        template<typename TCfg>
        TCfg read_toml(const std::string &configContent) {
            auto configStream = std::stringstream{configContent};
            return read_toml<TCfg>(configStream);
        }

        template<typename TCfg>
        TCfg read_toml(std::istream &configStream) {
            auto parser = toml::Parser{};
            return read<TCfg>(configStream, parser);
        }

        template<typename TCfg>
        TCfg read_ini_file(const turbo::filesystem::path &configFile) {
            auto parser = ini::Parser{};
            return read_file<TCfg>(configFile, parser);
        }

        template<typename TCfg>
        TCfg read_ini(const std::string &configContent) {
            auto configStream = std::stringstream{configContent};
            return read_ini<TCfg>(configStream);
        }

        template<typename TCfg>
        TCfg read_ini(std::istream &configStream) {
            auto parser = ini::Parser{};
            return read<TCfg>(configStream, parser);
        }

    private:
        void addNode(const std::string &name, std::unique_ptr<detail::INode> node) override {
            nodes_.emplace(detail::NameConverter<nameFormat>::name(name), std::move(node));
        }

        void addParam(const std::string &name, std::unique_ptr<detail::IParam> param) override {
            params_.emplace(detail::NameConverter<nameFormat>::name(name), std::move(param));
        }

        void addValidator(std::unique_ptr<detail::IValidator> validator) override {
            validators_.emplace_back((std::move(validator)));
        }

        void load(const TreeNode &treeNode) override {
            for (const auto &[nodeName, node]: treeNode.asItem().nodes()) {
                if (!nodes_.count(nodeName))
                    throw ConfigError{"Unknown node '" + nodeName + "'", node.position()};
                try {
                    nodes_.at(nodeName)->load(node);
                }
                catch (const detail::LoadingError &e) {
                    throw ConfigError{"Node '" + nodeName + "': " + e.what(), node.position()};
                }
            }

            for (const auto &[paramName, param]: treeNode.asItem().params()) {
                if (!params_.count(paramName))
                    throw ConfigError{"Unknown param '" + paramName + "'", param.position()};
                params_.at(paramName)->load(param);
            }

            checkLoadingResult();
        }

        void checkLoadingResult() {
            for (const auto &[name, param]: params_)
                if (!param->hasValue())
                    throw detail::LoadingError{"Parameter '" + name + "' is missing."};
            for (const auto &[name, node]: nodes_)
                if (!node->hasValue())
                    throw detail::LoadingError{"Node '" + name + "' is missing."};

            for (const auto &validator: validators_)
                validator->validate();
        }

        detail::ConfigReaderPtr makeNestedReader(const std::string &name) override {
            nestedReaders_.emplace(name, std::make_unique<ConfigReader<nameFormat>>());
            return nestedReaders_[name]->makePtr();
        }

        template<typename TCfg>
        TCfg read(std::istream &configStream, IParser &parser) {
            clear();
            if constexpr (!std::is_aggregate_v<TCfg>)
                static_assert(
                        std::is_constructible_v<TCfg, detail::ConfigReaderPtr>,
                        "Non aggregate config objects must inherit tconf::Config constructors with 'using "
                        "Config::Config;'");
            auto cfg = TCfg{makePtr()};
            auto tree = parser.parse(configStream);
            try {
                load(tree);
            }
            catch (const detail::LoadingError &e) {
                throw ConfigError{std::string{"Root node: "} + e.what(), tree.position()};
            }
            resetConfigReader(cfg);
            return cfg;
        }

        void clear() {
            nodes_.clear();
            params_.clear();
            nestedReaders_.clear();
            validators_.clear();
        }

    private:
        std::map<std::string, std::unique_ptr<detail::INode>> nodes_;
        std::map<std::string, std::unique_ptr<detail::IParam>> params_;
        std::map<std::string, std::unique_ptr<ConfigReader<nameFormat>>> nestedReaders_;
        std::vector<std::unique_ptr<detail::IValidator>> validators_;
    };

}  // namespace tconf

#endif  // TCONF_CONFIG_READER_H_
