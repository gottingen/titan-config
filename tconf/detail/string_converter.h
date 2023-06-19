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


#ifndef TCONF_STRINGCONVERTER_H
#define TCONF_STRINGCONVERTER_H

#include "tconf/errors.h"
#include "tconf/tree/string_converter.h"
#include <string>
#include <variant>

namespace tconf::detail {

struct StringConversionError {
    std::string message;
};

template<typename T>
std::variant<T, StringConversionError> convertFromString(const std::string& data)
{
    try {
        auto result = StringConverter<T>::fromString(data);
        if (!result.has_value())
            return StringConversionError{};
        else
            return result.value();
    }
    catch (const ValidationError& error) {
        return StringConversionError{error.what()};
    }
    catch (...) {
        return StringConversionError{};
    }
}

} //namespace tconf::detail

#endif //TCONF_STRINGCONVERTER_H
