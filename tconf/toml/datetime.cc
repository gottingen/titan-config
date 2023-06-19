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

#include <toml.hpp>
#include <tconf/toml/datetime.h>
#include <chrono>
#include <sstream>

namespace tconf {

    std::optional<toml::DateTimePoint> StringConverter<toml::DateTimePoint>::fromString(const std::string &data) {
        auto dateStr = "date = " + data;
        auto stream = std::stringstream{dateStr};
        const auto tomlData = ::toml::parse(stream);
        const auto date = ::toml::get<std::chrono::system_clock::time_point>(tomlData.at("date"));
        return toml::DateTimePoint{date};
    }

    std::optional<toml::TimeDuration> StringConverter<toml::TimeDuration>::fromString(const std::string &data) {
        auto timeStr = "time = " + data;
        auto stream = std::stringstream{timeStr};
        const auto tomlData = ::toml::parse(stream);
        const auto time = ::toml::get<std::chrono::seconds>(tomlData.at("time"));
        return toml::TimeDuration{time};
    }

} //namespace tconf
