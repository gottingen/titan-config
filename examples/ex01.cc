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


#include "tconf/config_reader.h"
#include "turbo/files/filesystem.h"
#include <iostream>
#include <vector>

struct ThumbnailCfg : public tconf::Config {
    int maxWidth = param<&ThumbnailCfg::maxWidth>();
    int maxHeight = param<&ThumbnailCfg::maxHeight>();
};

struct PhotoViewerCfg : public tconf::Config {
    //config fields can also be created with macros:
    TCONF_PARAM(rootDir, turbo::filesystem::path);
    TCONF_PARAM_LIST(supportedFiles, std::vector<std::string>);
    TCONF_NODE(thumbnailSettings, ThumbnailCfg);
};

int main() {
    auto cfgReader = tconf::ConfigReader{};
    auto cfg = cfgReader.read_toml<PhotoViewerCfg>(R"(
        rootDir = "~/Photos"
        supportedFiles = [".jpg", ".png"]
        [thumbnailSettings]
          maxWidth = 256
          maxHeight = 256
    )");
    //At this point your config is ready to use
    std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;

    if (!cfg.supportedFiles.empty())
        std::cout << "Supported files:" << std::endl;
    for (const auto &file: cfg.supportedFiles)
        std::cout << "  " << file << std::endl;

    std::cout << "Thumbnail settings:" << std::endl;
    std::cout << "  Max width:" << cfg.thumbnailSettings.maxWidth << std::endl;
    std::cout << "  Max height:" << cfg.thumbnailSettings.maxHeight << std::endl;
    return 0;
}