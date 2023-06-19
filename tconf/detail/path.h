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


#ifndef TCONF_SFUN_PATH_H
#define TCONF_SFUN_PATH_H

#include "tconf/detail/wstring_conv.h"
#include "turbo/files/filesystem.h"
#include <string>
#include <string_view>

namespace tconf::sfun {

    inline turbo::filesystem::path make_path(std::string_view str) {
#ifdef _WIN32
        return to_wstring(str);
#else
        return str;
#endif
    }

    inline std::string path_string(const turbo::filesystem::path &path) {
#ifdef _WIN32
        return from_wstring(path.wstring());
#else
        return path.string();
#endif
    }

} //namespace tconf::sfun

#endif //TCONF_SFUN_PATH_H
