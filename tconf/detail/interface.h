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

#ifndef TCONF_SFUN_INTERFACE_H
#define TCONF_SFUN_INTERFACE_H

#include <type_traits>

namespace tconf::sfun {

    template<typename T>
    class interface {
    public:
        interface() {
            static_assert(
                    std::is_base_of_v<interface<T>, T> && !std::is_convertible_v<T *, interface<T> *>,
                    "T must be derived from interface<T> privately");
        }

        virtual ~interface() = default;

        interface(const interface &) = delete;

        interface &operator=(const interface &) = delete;

        interface(interface &&) = delete;

        interface &operator=(interface &&) = delete;
    };

    template<typename T>
    struct access_token {
    private:
        access_token(T &) {};

        access_token() {};
        friend T;
    };

    namespace detail {

        template<typename T>
        struct AccessPermissionBase {
            void operator()(access_token<T>) {}
        };

    } //namespace detail

    template<typename... Ts>
    struct access_permission : private detail::AccessPermissionBase<Ts> ... {
        template<typename TAccessToken>
        access_permission(TAccessToken accessToken) {
            operator()(accessToken);
        }

        using detail::AccessPermissionBase<Ts>::operator()...;
    };

} //namespace tconf::sfun

#endif //TCONF_SFUN_INTERFACE_H