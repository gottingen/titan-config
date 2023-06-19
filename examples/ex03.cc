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
#include "tconf/short_macros.h" //enables macros without TCONF_ prefix
#include "turbo/files/filesystem.h"
#include <iostream>
#include <vector>
#include <map>

struct Host {
    std::string ip;
    int port;
};

namespace tconf {
    template<>
    struct StringConverter<Host> {
        static std::optional<Host> fromString(const std::string &data) {
            auto delimPos = data.find(':');
            if (delimPos == std::string::npos)
                return {};
            auto host = Host{};
            host.ip = data.substr(0, delimPos);
            host.port = std::stoi(data.substr(delimPos + 1, data.size() - delimPos - 1));
            return host;
        }
    };
}
struct SharedAlbumCfg : public tconf::Config {
    PARAM(dir, turbo::filesystem::path);
    PARAM(name, std::string);
    PARAM_LIST(hosts, std::vector<Host>)();
};
struct PhotoViewerCfg : public tconf::Config {
    PARAM(rootDir, turbo::filesystem::path);
    PARAM_LIST(supportedFiles, std::vector<std::string>);
    COPY_NODE_LIST(sharedAlbums, std::vector<SharedAlbumCfg>)();
    using StringMap = std::map<std::string, std::string>;
    DICT(envVars, StringMap)();
};

int main() {
    auto cfgReader = tconf::ConfigReader{};
    auto cfg = cfgReader.read_yaml<PhotoViewerCfg>(R"(
      rootDir: ~/Photos
      supportedFiles: [ ".jpg", "png"]
      sharedAlbums:
        -
          dir: "summer_2019"
          name: "Summer 2019"
          hosts: ["127.0.0.1:8080"]
    )");

    std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;

    if (!cfg.supportedFiles.empty())
        std::cout << "Supported files:" << std::endl;
    for (const auto &file: cfg.supportedFiles)
        std::cout << "  " << file << std::endl;

    if (!cfg.sharedAlbums.empty())
        std::cout << "Shared albums:" << std::endl;
    for (const auto &album: cfg.sharedAlbums) {
        std::cout << "  Album:" << album.name << std::endl;
        std::cout << "    Hosts:" << std::endl;
        for (const auto &host: album.hosts)
            std::cout << "      " << host.ip << ":" << host.port << std::endl;
    }

    return 0;
}
