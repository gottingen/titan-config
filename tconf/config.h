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

#ifndef TCONF_CONFIG_H_
#define TCONF_CONFIG_H_

#include "tconf/detail/config_macros.h"
#include "tconf/detail/dict.h"
#include "tconf/detail/dict_creator.h"
#include "tconf/detail/iconfig_reader.h"
#include "tconf/detail/initialized_optional.h"
#include "tconf/detail/inode.h"
#include "tconf/detail/ivalidator.h"
#include "tconf/detail/name_converter.h"
#include "tconf/detail/nodelist_creator.h"
#include "tconf/detail/param_creator.h"
#include "tconf/detail/paramlist_creator.h"
#include "tconf/detail/utils.h"
#include "tconf/tree/iparser.h"
#include "tconf/tree/tree.h"
#include "turbo/files/filesystem.h"
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "turbo/meta/reflect.h"

namespace tconf {
    namespace detail {
        class IParam;
    }

    class Config {

    public:
        Config() = default;

        Config(detail::ConfigReaderPtr reader)
                : cfgReader_{reader} {
        }

        Config(const Config &) = default;

        Config &operator=(const Config &) = default;

        Config(Config &&) {}

        Config &operator=(Config &&) {
            return *this;
        };

    protected:
        template<auto member>
        auto node(const std::string &memberName) {
            auto ptr = decltype(member){};
            return node<member>(ptr, memberName);
        }

        template<auto member>
        auto dict(const std::string &memberName) {
            auto ptr = decltype(member){};
            return dict<member>(ptr, memberName);
        }

        template<auto member>
        auto nodeList(const std::string &memberName) {
            auto ptr = decltype(member){};
            return nodeList<member>(ptr, memberName);
        }

        template<auto member>
        auto copyNodeList(const std::string &memberName) {
            auto ptr = decltype(member){};
            return copyNodeList<member>(ptr, memberName);
        }

        template<auto member>
        auto param(const std::string &memberName) {
            auto ptr = decltype(member){};
            return param<member>(ptr, memberName);
        }

        template<auto member>
        auto paramList(const std::string &memberName) {
            auto ptr = decltype(member){};
            return paramList<member>(ptr, memberName);
        }

        template<auto member>
        auto node() {
            return node<member>(std::string{turbo::nameof_member<member>()});
        }

        template<auto member>
        auto dict() {
            return dict<member>(std::string{turbo::nameof_member<member>()});
        }

        template<auto member>
        auto nodeList() {
            return nodeList<member>(std::string{turbo::nameof_member<member>()});
        }

        template<auto member>
        auto copyNodeList() {
            return copyNodeList<member>(std::string{turbo::nameof_member<member>()});
        }

        template<auto member>
        auto param() {
            return param<member>(std::string{turbo::nameof_member<member>()});
        }

        template<auto member>
        auto paramList() {
            return paramList<member>(std::string{turbo::nameof_member<member>()});
        }

        detail::ConfigReaderPtr cfgReader() const {
            return cfgReader_;
        }

    private:
        template<auto member, typename T, typename TCfg>
        auto node(T TCfg::*, const std::string &memberName) {
            auto cfg = static_cast<TCfg *>(this);
            return detail::NodeCreator<T>{cfgReader(), memberName, cfg->*member};
        }

        template<auto member, typename TMap, typename TCfg>
        auto dict(TMap TCfg::*, const std::string &memberName) {
            auto cfg = static_cast<TCfg *>(this);
            return detail::DictCreator<TMap>{cfgReader(), memberName, cfg->*member};
        }

        template<auto member, typename TCfgList, typename TCfg>
        auto nodeList(TCfgList TCfg::*, const std::string &memberName) {
            auto cfg = static_cast<TCfg *>(this);
            return detail::NodeListCreator<TCfgList>{cfgReader(), memberName, cfg->*member};
        }

        template<auto member, typename TCfgList, typename TCfg>
        auto copyNodeList(TCfgList TCfg::*, const std::string &memberName) {
            auto cfg = static_cast<TCfg *>(this);
            return detail::NodeListCreator<TCfgList>{cfgReader(), memberName, cfg->*member, detail::NodeListType::Copy};
        }

        template<auto member, typename T, typename TCfg>
        auto param(T TCfg::*, const std::string &memberName) {
            auto cfg = static_cast<TCfg *>(this);
            return detail::ParamCreator<T>{cfgReader(), memberName, cfg->*member};
        }

        template<auto member, typename T, typename TCfg>
        auto paramList(T TCfg::*, const std::string &memberName) {
            auto cfg = static_cast<TCfg *>(this);
            return detail::ParamListCreator<T>{cfgReader(), memberName, cfg->*member};
        }

    private:
        detail::ConfigReaderPtr cfgReader_;

        friend class detail::IConfigReader;
    };

    template<typename T>
    using optional = std::conditional_t<std::is_base_of_v<Config, T>, detail::InitializedOptional<T>, std::optional<T>>;

} //namespace tconf

#endif  // TCONF_CONFIG_H_
