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
#include "tconf/detail/param.h"
#include "tconf/detail/utils.h"
#include "tconf/detail/type_traits.h"
#include <tconf/tree/tree.h>
#include <map>
#include <string>
#include <type_traits>

namespace tconf::detail {

    template<typename TMap>
    class Dict : public INode {
    public:
        explicit Dict(std::string name, TMap &dictMap)
                : name_{std::move(name)}, dictMap_{dictMap} {
            static_assert(
                    sfun::is_associative_container_v < sfun::remove_optional_t < TMap >> ,
                    "Dictionary field must be an associative container or an associative container placed in "
                    "std::optional");
            static_assert(
                    std::is_same_v<typename sfun::remove_optional_t<TMap>::key_type, std::string>,
                    "Dictionary associative container's key type must be std::string");
        }

        void markValueIsSet() {
            hasValue_ = true;
        }

    private:
        void load(const TreeNode &node) override {
            hasValue_ = true;
            position_ = node.position();
            if (!node.isItem())
                throw ConfigError{"Dictionary '" + name_ + "': config node can't be a list.", node.position()};
            if constexpr (sfun::is_optional_v < TMap >)
                dictMap_.emplace();
            maybeOptValue(dictMap_).clear();

            for (const auto &[paramName, paramValue]: node.asItem().params()) {
                using Param = typename sfun::remove_optional_t<TMap>::mapped_type;
                const auto paramNameStr = paramName;
                const auto paramValueStr = paramValue;
                auto paramReadResult = convertFromString<Param>(paramValueStr.value());
                auto readResultVisitor = sfun::overloaded{
                        [&](const Param &param) {
                            maybeOptValue(dictMap_).emplace(paramNameStr, param);
                        },
                        [&](const StringConversionError &error) {
                            throw ConfigError{
                                    "Couldn't set dict element'" + name_ + "' value from '" + paramValueStr.value() +
                                    "'" +
                                    (!error.message.empty() ? ": " + error.message : ""),
                                    position_};
                        }};
                std::visit(readResultVisitor, paramReadResult);
            }
        }

        bool hasValue() const override {
            if constexpr (sfun::is_optional_v < TMap >)
                return true;
            else
                return hasValue_;
        }

        StreamPosition position() override {
            return position_;
        }

        std::string description() override {
            return "Dictionary '" + name_ + "'";
        }

    private:
        std::string name_;
        TMap &dictMap_;
        bool hasValue_ = false;
        StreamPosition position_;
    };

} //namespace tconf::detail