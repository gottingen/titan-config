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

#ifndef TCONF_SFUN_WSTRINGCONV_H
#define TCONF_SFUN_WSTRINGCONV_H

#ifdef _WIN32
#include "detail/external/utf8.h"
#endif

#include "tconf/detail/utility.h"
#include <string>

namespace tconf::sfun {

    template<typename T = void>
    inline std::string from_wstring([[maybe_unused]] std::wstring_view utf16String) {
#ifdef _WIN32
        auto result = std::string{};
        utf8::utf16to8(utf16String.begin(), utf16String.end(), std::back_inserter(result));
        return result;
#else
        static_assert(
                dependent_false<T>,
                "In order to avoid misuse, the fromWString() function can only be used on the Windows platform.");
        return {};
#endif
    }

    template<typename T = void>
    inline std::wstring to_wstring([[maybe_unused]] std::string_view utf8String) {
#ifdef _WIN32
        auto result = std::wstring{};
        utf8::utf8to16(utf8String.begin(), utf8String.end(), std::back_inserter(result));
        return result;
#else
        static_assert(
                dependent_false<T>,
                "In order to avoid misuse, the toWString() function can only be used on the Windows platform.");
        return {};
#endif
    }

} //namespace tconf::sfun

#endif //TCONF_SFUN_WSTRINGCONV_H
