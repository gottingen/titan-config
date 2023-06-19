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
#include "tconf/ini/param_parser.h"
#include "tconf/ini/inicpp.h"
#include "tconf/ini/parser.h"
#include "tconf/errors.h"
#include "tconf/tree/tree.h"
#include "tconf/detail/string_utils.h"
#include <iterator>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace tconf::ini {

    namespace {
        const auto fakeRootSectionId = std::string{"71cd27eb-7f27-4b61-a2ff-a64c89b275a7"};

        bool isUnsignedInt(const std::string &str) {
            return std::all_of(str.begin(), str.end(), ::isdigit);
        }

        bool hasRootSection(std::istream &stream) {
            auto pos = stream.tellg();
            auto firstWord = std::string{};
            stream >> firstWord;
            stream.seekg(pos);
            return firstWord.front() == '[';
        }

        std::string getStreamContent(std::istream &stream) {
            stream >> std::noskipws;
            return {std::istream_iterator<char>{stream}, std::istream_iterator<char>{}};
        }

        void parseSection(const ::ini::IniSection &section, tconf::TreeNode &node) {
            for (const auto &[key, value]: section) {
                auto paramList = detail::readParamList(key, value.as<std::string>());
                if (paramList)
                    node.asItem().addParamList(key, *paramList);
                else
                    node.asItem().addParam(key, detail::readParam(value.as<std::string>()));
            }
        }

        std::string beforeLast(const std::string &str, const std::string &delim) {
            auto pos = str.rfind(delim);
            if (pos == std::string::npos)
                return str;
            return str.substr(0, pos);
        }

        tconf::ConfigError makeConfigError(const std::exception &e, bool hasFakeRootSection) {
            auto message = std::string{e.what()};
            auto regex = std::regex{R"(l\.(\d+): (.*))"};
            auto match = std::smatch{};
            if (std::regex_search(message, match, regex)) {
                auto line = std::stoi(match[1]);
                if (hasFakeRootSection)
                    line--;
                auto error = match[2].str();
                if (!error.empty())
                    error[0] = static_cast<char>(std::toupper(error[0]));

                return {error, {line, {}}};
            }
            return {e.what()};
        }
    } //namespace

    class Parser::Impl {
    public:
        void parseSections(const ::ini::IniFile &ini, tconf::TreeNode &node);

        void createSectionNodes(const ::ini::IniFile &ini, tconf::TreeNode &node);

        void addSectionTreeNode(
                const std::string &sectionName,
                const std::function<tconf::TreeNode *(tconf::TreeNode &)> &nodeMaker);

    private:
        std::map<std::string, int> sectionArrays_;
        std::map<std::string, tconf::TreeNode *> sections_;
        tconf::TreeNode *currentNode_ = nullptr;
    };

    Parser::Parser()
            : impl_{std::make_unique<Parser::Impl>()} {
    }

    Parser::~Parser() = default;

    TreeNode Parser::parse(std::istream &stream) {
        auto fixedInputStream = std::unique_ptr<std::istream>{};
        auto &input = [&]() -> decltype(auto) {
            if (hasRootSection(stream))
                return stream;
            auto inputStr = "[" + fakeRootSectionId + "]\n" + getStreamContent(stream);
            fixedInputStream = std::make_unique<std::istringstream>(inputStr);
            return *fixedInputStream;
        }();

        auto ini = [&] {
            try {
                return ::ini::IniFile{input};
            }
            catch (const std::exception &e) {
                throw makeConfigError(e, fixedInputStream != nullptr);
            }
        }();

        auto tree = tconf::makeTreeRoot();
        impl_->createSectionNodes(ini, tree);
        impl_->parseSections(ini, tree);
        return tree;
    }

    void Parser::Impl::parseSections(const ::ini::IniFile &ini, tconf::TreeNode &node) {
        for (const auto &[sectionName, section]: ini)
            if (sectionName == fakeRootSectionId)
                parseSection(section, node);
            else if (!sectionArrays_.count(sectionName))
                parseSection(section, *sections_.at(sectionName));
    }

    void Parser::Impl::createSectionNodes(const ::ini::IniFile &ini, tconf::TreeNode &node) {
        for (const auto &[sectionName, section]: ini) {
            currentNode_ = &node;
            if (sectionName == fakeRootSectionId)
                continue;
            auto newSectionName = std::string{};
            auto sectionParts = sfun::split(sectionName, ".");
            for (auto sectionPartIt = sectionParts.begin(); sectionPartIt != sectionParts.end(); ++sectionPartIt) {
                auto sectionPart = std::string{*sectionPartIt};
                newSectionName += sectionPart;
                if (std::next(sectionPartIt) != sectionParts.end() &&
                    isUnsignedInt(std::string{*std::next(sectionPartIt)})) {
                    addSectionTreeNode(
                            newSectionName,
                            [&](auto &parentNode) {
                                sectionArrays_[newSectionName] = 0;
                                return &parentNode.asItem().addNodeList(sectionPart);
                            });
                } else if (isUnsignedInt(sectionPart)) {
                    addSectionTreeNode(
                            newSectionName,
                            [&](auto &parentNode) {
                                auto listSectionName = beforeLast(newSectionName, ".");
                                auto sectionArrayIndex = std::stoi(sectionPart);
                                if (sectionArrayIndex != sectionArrays_[listSectionName]) {
                                    if (sectionArrayIndex != ++sectionArrays_[listSectionName])
                                        throw tconf::ConfigError{"Section array index mismatch"};
                                }
                                return &parentNode.asList().addNode();
                            });
                } else {
                    addSectionTreeNode(
                            newSectionName,
                            [&](auto &parentNode) {
                                return &parentNode.asItem().addNode(sectionPart);
                            });
                }
                newSectionName += ".";
            }
        }
    }

    void Parser::Impl::addSectionTreeNode(
            const std::string &sectionName,
            const std::function<tconf::TreeNode *(tconf::TreeNode &)> &nodeMaker) {
        if (!sections_.count(sectionName))
            sections_[sectionName] = nodeMaker(*currentNode_);
        currentNode_ = sections_[sectionName];
    }

} //namespace tconf::ini
