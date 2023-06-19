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

#ifndef TCONF_TOML_DATETIME_H_
#define TCONF_TOML_DATETIME_H_

#include "tconf/detail/string_converter.h"
#include <chrono>
#include <sstream>

namespace tconf::toml {
    struct TimeDuration {
        std::chrono::seconds value;
    };
    struct DateTimePoint {
        std::chrono::system_clock::time_point value;
    };
} //namespace tconf::toml

namespace tconf {
    template<>
    struct StringConverter<toml::DateTimePoint> {
        static std::optional<toml::DateTimePoint> fromString(const std::string &data);
    };

    template<>
    struct StringConverter<toml::TimeDuration> {
        static std::optional<toml::TimeDuration> fromString(const std::string &data);
    };

} //namespace tconf

#endif  // TCONF_TOML_DATETIME_H_