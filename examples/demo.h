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

#include "tconf/config.h"
#include "tconf/short_macros.h" //enables macros without TCONF_ prefix
#include <string>
#include <vector>
#include <map>
#include "turbo/files/filesystem.h"

struct ThumbnailCfg : public tconf::Config {
    PARAM(enabled, bool)(true);
    PARAM(maxWidth, int);
    PARAM(maxHeight, int);
};
struct HostCfg : public tconf::Config {
    PARAM(ip, std::string);
    PARAM(port, int);
};
struct SharedAlbumCfg : public tconf::Config {
    PARAM(dir, turbo::filesystem::path);
    PARAM(name, std::string);
    NODE_LIST(hosts, std::vector<HostCfg>)();
};
struct PhotoViewerCfg : public tconf::Config {
    PARAM(rootDir, turbo::filesystem::path);
    PARAM_LIST(supportedFiles, std::vector<std::string>);
    NODE(thumbnails, ThumbnailCfg);
    COPY_NODE_LIST(sharedAlbums, std::vector<SharedAlbumCfg>)();
    using StringMap = std::map<std::string, std::string>;
    DICT(envVars, StringMap)();
};