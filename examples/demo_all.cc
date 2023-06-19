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


#include "demo.h"
#include "print_demo.h"
#include "tconf/config_reader.h"
#include "turbo/files/filesystem.h"
#include <iostream>

namespace fs = turbo::filesystem;

PhotoViewerCfg readCfg(const fs::path &path) {
    auto cfgReader = tconf::ConfigReader{};
    if (path.extension() == ".ini")
        return cfgReader.read_ini_file<PhotoViewerCfg>(turbo::filesystem::weakly_canonical(path));
    else if (path.extension() == ".json")
        return cfgReader.read_json_file<PhotoViewerCfg>(turbo::filesystem::weakly_canonical(path));
    else if (path.extension() == ".toml")
        return cfgReader.read_toml_file<PhotoViewerCfg>(turbo::filesystem::weakly_canonical(path));
    else if (path.extension() == ".yaml")
        return cfgReader.read_yaml_file<PhotoViewerCfg>(turbo::filesystem::weakly_canonical(path));
    else
        throw std::runtime_error{"unknown format"};
}

int mainApp(const turbo::filesystem::path &cfgPath) {
    try {
        auto cfg = readCfg(cfgPath);
        std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;
        printDemoConfig(cfg);
    }
    catch (tconf::ConfigError &error) {
        std::cout << "Configuration error: " << error.what() << std::endl;
        return 1;
    }
    return 0;
}

#ifdef _WIN32
int wmain(int argc, wchar_t** argv)
{
    if (argc < 2)
        throw std::runtime_error{"A path to the config file must be specified in the command line"};
    auto cfgPath = turbo::filesystem::path{argv[1]};
    return mainApp(cfgPath);
}
#else

int main(int argc, char **argv) {
    if (argc < 2)
        throw std::runtime_error{"A path to the config file must be specified in the command line"};
    auto cfgPath = turbo::filesystem::path{argv[1]};
    return mainApp(cfgPath);
}

#endif