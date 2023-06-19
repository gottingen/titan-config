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
#include "tconf/tree/iparser.h"
#include "tconf/tree/tree.h"
#include "tconf/errors.h"
#include "tconf/config_reader.h"
#include <iostream>

class DemoTreeProvider : public tconf::IParser
{
    tconf::TreeNode parse(std::istream& stream) final
    {
        auto tree = tconf::makeTreeRoot();
        tree.asItem().addParam("rootDir", "~/Photos");
        tree.asItem().addParamList("supportedFiles", {".jpg", ".png"});

        auto& thumbNode = tree.asItem().addNode("thumbnails");
        thumbNode.asItem().addParam("enabled", "1");
        thumbNode.asItem().addParam("maxWidth", "128");
        thumbNode.asItem().addParam("maxHeight", "128");

        auto& albumsNodeList = tree.asItem().addNodeList("sharedAlbums");
        auto& albumNode = albumsNodeList.asList().addNode();
        albumNode.asItem().addParam("dir", "summer_2019");
        albumNode.asItem().addParam("name", "Summer (2019)");
        auto& hostsNodeList = albumNode.asItem().addNodeList("hosts");
        auto& hostNode = hostsNodeList.asList().addNode();
        hostNode.asItem().addParam("ip", "127.0.0.1");
        hostNode.asItem().addParam("port", "80");

        //For error notifications use tconf::ConfigError exceptions
        if (stream.bad())
            throw tconf::ConfigError{"stream error"};

        //Stream position information can be added to objects
        auto pos = tconf::StreamPosition{13, 1};
        auto& envVarsNode = tree.asItem().addNode("envVars", pos);
        envVarsNode.asItem().addParam("DISPLAY", "0.1");

        //and to ConfigError exceptions
        if (stream.fail())
            throw tconf::ConfigError{"stream error", pos};

        return tree;
    }
};

int main()
{
    auto cfgReader = tconf::ConfigReader{};
    auto parser = DemoTreeProvider{};
    auto cfg = cfgReader.read<PhotoViewerCfg>("", parser);
    std::cout << "Launching PhotoViewer in directory " << cfg.rootDir;
    return 0;
}