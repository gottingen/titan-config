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

#ifndef TCONFI_TREE_STRING_CONVERTER_H_
#define TCONFI_TREE_STRING_CONVERTER_H_

#include "tconf/errors.h"
#include "tconf/detail/type_traits.h"
#include <optional>
#include <sstream>
#include <string>

namespace tconf {

    template<typename T>
    struct StringConverter {
        static std::optional<T> fromString(const std::string &data) {
            [[maybe_unused]] auto setValue = [](auto &value, const std::string &data) -> std::optional<T> {
                auto stream = std::stringstream{data};
                stream >> value;

                if (stream.bad() || stream.fail() || !stream.eof())
                    return {};
                return value;
            };

            if constexpr (std::is_convertible_v<std::string, sfun::remove_optional_t < T>>) {
                return data;
            }
            else if constexpr (sfun::is_optional_v < T >) {
                auto value = T{};
                value.emplace();
                return setValue(*value, data);
            } else {
                auto value = T{};
                return setValue(value, data);
            }
        }
    };

} //namespace tconf

#endif // TCONFI_TREE_STRING_CONVERTER_H_
