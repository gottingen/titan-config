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
#include <iostream>

int main() {
    auto cfgReader = tconf::ConfigReader{};
    auto cfg = cfgReader.read_yaml_file<PhotoViewerCfg>(turbo::filesystem::canonical("demo.yaml"));
    std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;
    printDemoConfig(cfg);

    return 0;
}