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

#ifndef TCONF_ERRORS_H_
#define TCONF_ERRORS_H_

#include "tconf/tree/stream_position.h"
#include <stdexcept>
#include <string>

namespace tconf {

    class Error : public std::runtime_error {
    public:
        explicit Error(const std::string &errorMsg)
                : std::runtime_error(errorMsg) {
        }
    };

    class ConfigError : public Error {
    public:
        using Error::Error;

        ConfigError(const std::string &errorMsg, const StreamPosition &errorPosition = {})
                : Error(streamPositionToString(errorPosition) + errorMsg) {
        }
    };

    class ValidationError : public Error {
        using Error::Error;
    };

}  // namespace tconf

#endif  // TCONF_ERRORS_H_
