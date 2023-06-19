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
#include "tconf/short_macros.h"
#include "turbo/files/filesystem.h"
#include <iostream>
#include <map>
#include <vector>

struct NotEmpty {
    template<typename TList>
    void operator()(const TList &list) {
        if (!list.empty())
            throw tconf::ValidationError{"can't be empty."};
    }
};

struct PhotoViewerCfg : public tconf::Config {
    PARAM(rootDir, turbo::filesystem::path)
    .ensure(
            [](const auto &path) {
                if (!turbo::filesystem::exists(path))
                    throw tconf::ValidationError{"a path must exist"};
            });
    PARAM_LIST(supportedFiles, std::vector<std::string>).ensure<NotEmpty>();
    using StringMap = std::map<std::string, std::string>;
    DICT(envVars, StringMap)();
};

int main() {
    try {
        auto cfgReader = tconf::ConfigReader{};
        auto cfg = cfgReader.read_yaml<PhotoViewerCfg>(R"(
            rootDir: ~/Photos
            supportedFiles: []
        )");

        std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;

        if (!cfg.supportedFiles.empty())
            std::cout << "Supported files:" << std::endl;
        for (const auto &file: cfg.supportedFiles)
            std::cout << "  " << file << std::endl;

        return 0;
    }
    catch (const tconf::ConfigError &e) {
        std::cout << "Config error:" << e.what();
        return 1;
    }
}