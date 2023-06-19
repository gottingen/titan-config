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

#include "tconf/detail/initialized_optional.h"
#include "tconf/detail/type_traits.h"
#include <optional>
#include <type_traits>
#include <vector>

namespace tconf::detail {

    template<typename T, typename = void>
    struct is_initialized_optional : std::false_type {
    };

    template<typename T>
    struct is_initialized_optional<tconf::detail::InitializedOptional<T>> : std::true_type {
    };

    template<typename T>
    inline constexpr auto is_initialized_optional_v = is_initialized_optional<T>::value;

    template<typename T>
    auto &maybeOptValue(T &obj) {
        if constexpr (sfun::is_optional_v<T>)
            return *obj;
        else
            return obj;
    }

} //namespace tconf::detail

namespace tconf::sfun {
    template<typename T>
    struct remove_optional<tconf::detail::InitializedOptional<T>> {
        using type = T;
    };
} //namespace tconf::sfun