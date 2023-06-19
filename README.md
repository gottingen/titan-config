titan-conf
======

`tconf` - is a C++17 library, providing a convenient declarative interface for configuration parsers and built-in support 
for reading `JSON`, `YAML`, `TOML`,and  `INI` config files. To use it, create a configuration schema by declaring a structure 
for each level of your config file and load it by calling a method, matching the preferred configuration format:

```C++
///examples/ex01.cc
///

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

```

## Table of Contents
* [Usage](#usage)
    * [Config structure](#Config-structure)
    * [Supporting non-aggregate config structures](#supporting-non-aggregate-config-structures)
    * [Registration without macros](#registration-without-macros)
    * [Supported formats](#supported-formats)
        * [JSON](#json)
        * [YAML](#yaml)
        * [TOML](#toml)
        * [INI](#ini)
    * [Creation of tconf-compatible parsers](#creation-of-tconf-compatible-parsers)
    * [User defined types](#user-defined-types)
    * [Validators](#validators)
* [Installation](#installation)
* [Running tests](#running-tests)
* [Building examples](#building-examples)
* [License](#license)

## Usage


### Config structure

To use `tconf`, you need to create a structure with fields corresponding to the config's parameters.  
To do this, subclass `tconf::Config` and declare fields using the following macros:

- **TCONF_PARAM(`name`, `type`)** - creates a `type name;` config field and registers it in the parser.
- **TCONF_PARAMLIST(`name`, `listType`)** - creates a `listType name;` config field and registers it in the parser. listType can be any sequence container that supports the `emplace_back` operation, such as `vector`, `deque`, or `list` from the STL.
- **TCONF_NODE(`name`, `type`)** - creates a `type name;` config field for a nested configuration structure and registers it in the parser. The type of the name field must be a subclass of `tconf::Config`.
- **TCONF_NODELIST(`name`, `listType`)** - creates a `listType name;` config field for a list of nested configuration structures and registers it in the parser. `listType` can be any sequence container that supports the `emplace_back` operation, such as `vector`, `deque`, or `list` from the STL. The type stored in the list (listType::value_type) must be a subclass of `tconf::Config`.
- **TCONF_COPY_NODELIST(`name`, `listType`)** - creates a `listType name;` config field for a list of nested configuration structures and registers it in the parser. `listType` can be any sequence container that supports the `emplace_back` operation, such as `vector`, `deque`, or `list` from the STL. The type stored in the list (listType::value_type) must be a subclass of `tconf::Config`. The first element of this list acts as a template for the other elements, which means that all unspecified parameters of the second and following elements will be copied from the first element without raising a parsing error for missing parameters.
- **TCONF_DICT(`name`, `mapType`)** - creates a `mapType name;` config field for a nested dictionary and registers it in the parser. `mapType` can be any associative container that supports the emplace operation, such as `map` or `unordered_map` from the STL. The key type of the map must be `std::string`  
  The preprocessor doesn't handle commas between template arguments in the correct way, so you need to create an alias for your map in order to use it with this macro:
 ```c++
    using StringMap = std::map<std::string, std::string>;
    TCONF_DICT(testDict, StringMap);
 ```

Notes:
- All config entities listed above provide the parenthesis operator `()` which sets the default value and makes this config field optional. This means that the field can be omitted from the configuration file without raising an error. The empty `operator ()` makes a field's value default initialized, otherwise the passed parameters are used for initialization. `TCONF_NODE`, `TCONF_NODELIST`, and `TCONF_COPY_NODELIST` only support default initialization.
- It is also possible to make any config field optional by placing it in `tconf::optional` (a `std::optional`-like wrapper with a similar interface). If a value for this field is missing from the config file, the field remains uninitialized and no error occurs.
- Types used for config parameters must be default constructible and copyable.

You do not need to change your code style when declaring config fields. `camelCase`, `snake_case`, and `PascalCase` names are supported, and can be converted to the format used by parameter names in the config file. To do this, specify the configuration names format with the `tconf::NameFormat` enum by passing its value to the `tconf::ConfigReader` template argument.

To demonstrate it, let's change our PhotoViewer example to use snake_case names in the configuration:

```C++
///examples/ex02.cc
///

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
auto cfgReader = tconf::ConfigReader<tconf::NameFormat::SnakeCase>{};
auto cfg = cfgReader.read_toml<PhotoViewerCfg>(R"(
        root_dir = "/home/kamchatka-volcano/photos"
        supported_files = [".jpg", ".png"]
        [thumbnail_settings]
          max_width = 256
          max_height = 256
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
```

### Supporting non-aggregate config structures
`tconf` relies on aggregate initialization of user-provided structures. If your config object needs to contain private data or virtual functions, it becomes a non-aggregate type. In this case, you must use the following `using` declaration to inherit `tconf::Config`'s constructors: `using Config::Config;`
```cpp
struct PhotoViewerCfg : public tconf::Config
{
    using Config::Config;
    virtual ~PhotoViewerCfg() = default; //virtual destructor makes PhotoViewerCfg non-aggregate
    TCONF_PARAM(rootDir, std::filesystem::path);
    TCONF_PARAMLIST(supportedFiles, std::vector<std::string>);
    TCONF_NODE(thumbnailSettings, ThumbnailCfg);
};
```

### Registration without macros
`tconf` can be used without macros, as every configuration entity described earlier can be registered with the similarly named `tconf::Config`'s member templates:
```c++
    struct Cfg : public tconf::Config{
        int testParam                      = param<&Cfg::testParam>();
        int testParam2                     = param<&Cfg::testParam2>()(100);
        tconf::optional<int> testParam3      = param<&Cfg::testParam3>();
        std::vector<double> testParamList  = paramList<&Cfg::testParamList>();
        TestNode testNode                  = node<&Cfg::testNode>();
        tconf::optional<TestNode> testNode2  = node<&Cfg::testNode2>();
        std::vector<TestNode> testNodeList = nodeList<&Cfg::testNodeList>();
        std::vector<TestNode> copyTestNodeList = copyNodeList<&Cfg::copyTestNodeList>();
        std::map<std::string, std::string> testDict = dict<&Cfg::testDict>();
    };
```

```c++
    struct Cfg : public tconf::Config{
        int testParam                          = param<&Cfg::testParam>("testParam");
        int testParam2                         = param<&Cfg::testParam2>("testParam2")(100);
        tconf::optional<int> testParam3      = param<&Cfg::testParam3>("testParam3");
        std::vector<double> testParamList      = paramList<&Cfg::testParamList>("testParamList");
        TestNode testNode                      = node<&Cfg::testNode>("testNode");
        tconf::optional<TestNode> testNode2  = node<&Cfg::testNode2>("testNode2");
        std::vector<TestNode> testNodeList     = nodeList<&Cfg::testNodeList>("testNodeList");
        std::vector<TestNode> copyTestNodeList = copyNodeList<&Cfg::copyTestNodeList>("copyTestNodeList");
        std::map<std::string, std::string> testDict = dict<&Cfg::testDict>("testDict");
    };
```

Config structures declared using the macro-free methods are fully compatible with all of `tconf`'s functionality. 
Examples in the documentation use registration with macros, as it is the least verbose method.


### Supported formats

Internally, the `tconf` library works on a tree-like structure provided by the [`tconf_tree`](tconf/tree) library, 
and it is not aware of different configuration formats. The user needs to provide a parser implementing the `tconf_tree::IParser` 
interface to convert a configuration file to a tree structure based on the `tconf_tree::TreeNode` class. It is also possible to 
create a `tconf` compatible parser adapter that transforms the parsing result of some 3rd party configuration parsing library to a 
tree using `tconf_tree::TreeNode`. Five such adapters for popular configuration formats are included in `tconf`, and are fetched and 
built into a static library called `tconf_formats` which is automatically configured and linked by `tconf`'s CMake configuration.

Let's increase the complexity of our example config to demonstrate how configuration elements work with each format:

#### demo.h
```C++
///examples/demo.h
///
#pragma once
#include <tconf/config.h>
#include <tconf/shortmacros.h> //enables macros without TCONF_ prefix
#include <string>
#include <vector>
#include <map>

struct ThumbnailCfg : public tconf::Config
{
    PARAM(enabled, bool)(true);
    PARAM(maxWidth, int);
    PARAM(maxHeight, int);
};
struct HostCfg : public tconf::Config{
    PARAM(ip, std::string);
    PARAM(port, int);
};
struct SharedAlbumCfg : public tconf::Config{
    PARAM(dir, std::filesystem::path);
    PARAM(name, std::string);
    NODELIST(hosts, std::vector<HostCfg>)();
};
struct PhotoViewerCfg : public tconf::Config{
    PARAM(rootDir, std::filesystem::path);
    PARAMLIST(supportedFiles, std::vector<std::string>);
    NODE(thumbnails, ThumbnailCfg);
    COPY_NODELIST(sharedAlbums, std::vector<SharedAlbumCfg>)();
    using StringMap = std::map<std::string, std::string>;
    DICT(envVars, StringMap)();
};
```

#### JSON

JSON support is provided by [`nlohmann/json`](https://github.com/nlohmann/json) library which is fetched and adapted to the `tconf` interface by the [`tconf_json`](https://github.com/gottingen/titan-conf_json) library.

A JSON config that matches the configuration listed in [`demo.h`](#demoh) earlier, looks like this:

`demo.json`
```json
{
  "rootDir": "~/Photos",
  "supportedFiles": [
    ".jpg",
    "png"
  ],
  "thumbnails": {
    "enabled": "1",
    "maxWidth": "128",
    "maxHeight": "128"
  },
  "sharedAlbums": [
    {
      "dir": "summer_2019",
      "name": "Summer (2019)",
      "hosts" : [{"ip" : "127.0.0.1", "port" : "8080" }]
    },
    {
      "dir": "misc",
      "name": "Misc"
    }
  ],
  "envVars": {
    "DISPLAY" : "0.1"
  }
}
```

```C++
///examples/demo_json.cc
///
#include "demo.h"
#include <tconf/config_reader.h>
#include <iostream>

int main()
{
    auto cfgReader = tconf::ConfigReader{};
    auto cfg = cfgReader.readJsonFile<PhotoViewerCfg>(std::filesystem::canonical("../../examples/demo.json"));
    std::cout << "Launching PhotoViewer in directory" << cfg.rootDir << std::endl;
    return 0;
}
```


#### YAML

YAML support is provided by the [`rapidyaml`](https://github.com/biojppm/rapidyaml) library which is fetched and adapted to the `tconf` interface by the [`tconf_yaml`](https://github.com/gottingen/titan-conf_yaml) library.

A YAML config that matches the configuration listed in [`demo.h`](#demoh) earlier, looks like this:  
`demo.yaml`
```yaml
  rootDir: ~/Photos
  supportedFiles: [ ".jpg", "png"]
  thumbnails:
    enabled: 1
    maxWidth: 128
    maxHeight: 128
  
  sharedAlbums:
    -
      dir: "summer_2019"
      name: "Summer (2019)"
      hosts: 
        -
          ip: "127.0.0.1"
          port: 8080
    -    
      dir: "misc"
      name: "Misc"      
  envVars: 
    DISPLAY : "0.1"
```
```c++
///examples/demo_yaml.cc
///
#include "demo.h"
#include <tconf/config_reader.h>
#include <iostream>

int main()
{
    auto cfgReader = tconf::ConfigReader{};
    auto cfg = cfgReader.readYamlFile<PhotoViewerCfg>(std::filesystem::canonical("../../examples/demo.yaml"));
    std::cout << "Launching PhotoViewer in directory" << cfg.rootDir << std::endl;
    return 0;
}
```
Notes:
* `tconf_yaml` supports only a single document section (starting with `---`) in your YAML config file.

#### TOML

TOML support is provided by the [`toml11`](https://github.com/ToruNiina/toml11) library which is fetched and adapted to the `tconf` interface by the [`tconf_toml`](https://github.com/gottingen/titan-conf_toml) library.

A TOML config that matches the configuration listed in [`demo.h`](#demoh) earlier, looks like this:  
`demo.toml`
```toml
  rootDir = "~/Photos"
  supportedFiles = [ ".jpg", "png"]
  [thumbnails]
    enabled = 1
    maxWidth = 128
    maxHeight = 128
  
  [[sharedAlbums]]
    dir  = "summer_2019"
    name = "Summer (2019)"
    [[sharedAlbums.hosts]]
      ip = "127.0.0.1"
      port = 8080 
  [[sharedAlbums]]
      dir  = "misc"
      name = "Misc"      
  [envVars] 
    DISPLAY = "0.1"
```
```c++
///examples/demo_toml.cc
///
#include "demo.h"
#include <tconf/config_reader.h>
#include <iostream>

int main()
{
    auto cfgReader = tconf::ConfigReader{};
    auto cfg = cfgReader.readTomlFile<PhotoViewerCfg>(std::filesystem::canonical("../../examples/demo.toml"));
    std::cout << "Launching PhotoViewer in directory" << cfg.rootDir << std::endl;
    return 0;
}
```
Notes:
* TOML nested arrays aren't supported, as they can't be represented within `tconf` configuration tree.
* TOML datetime types can be used in the config by including `<tconf/format/toml/datetime.h>` header:
```C++
#include <tconf/config.h>
#include <tconf/format/toml/datetime.h>

struct Cfg: tconf::Config
{   
    FICONE_PARAM(date, tconf::toml::DateTimePoint);
    FICONE_PARAM(time, tconf::toml::TimeDuration);
};
```

`tconf::toml::DateTimePoint` is used to read TOML `local_date (2018-12-23)`,
`local_datetime (2018-12-23T12:30:00)` and `offset_datetime (2018-12-23T12:30:00+09:30)` values.  
`tconf::toml::TimeDuration` is used to read TOML `time (12:30:00)` values.


#### INI

INI support is provided by the [`inifile-cpp`](https://github.com/Rookfighter/inifile-cpp) library which is fetched and adapted to the `tconf` interface by the [`tconf_ini`](https://github.com/gottingen/titan-conf_ini) library.

An INI config that matches the configuration listed in [`demo.h`](#demoh) earlier, looks like this:  
`demo.ini`
```ini
rootDir = "~/Photos"
supportedFiles = [".jpg", "png"]
[thumbnails]
  enabled = 1
  maxWidth = 128
  maxHeight = 12
[sharedAlbums.0]
  dir  = "summer_2019"
  name = "Summer (2019)"
  [sharedAlbums.0.hosts.0]
    ip = "127.0.0.1"
    port = 8080
[sharedAlbums.1]
  dir  = "misc"
  name = "Misc"
[envVars]
DISPLAY = "0.1"
```
```c++
///examples/demo_ini.cc
///
#include "demo.h"
#include <tconf/config_reader.h>
#include <iostream>

int main()
{
    auto cfgReader = tconf::ConfigReader{};
    auto cfg = cfgReader.readIniFile<PhotoViewerCfg>(std::filesystem::canonical("../../examples/demo.ini"));
    std::cout << "Launching PhotoViewer in directory" << cfg.rootDir << std::endl;
    return 0;
}
```
Notes:
* Nested structures can be created by using dotted ini section names listing the full hierarchy: `[grandparent.parent.child]`
* Node lists can be created by using indices in dotted ini section names, starting with zero for the first element: `[nodelist.0]`
* Parameter lists have the format: `[value1, value2, ...]`
* Multiline values aren't supported.


### Creation of tconf-compatible parsers
To create a parser compatible with `tconf`, you will need to use the [`tconf_tree`](https://github.com/gottingen/titan-conf_tree) 
library, which provides all the necessary types and interfaces for this task. The parsing class should implement the `tconf::IParser` 
interface and return the result of the configuration parsing in the form of a tree-like structure, constructed using `tconf::TreeNode` 
and `tconf::TreeParam` objects. Let's demonstrate how to work with the `tconf_tree` library by creating a fake parser that provides a 
configuration tree for the demo structure listed in [`demo.h`](#demoh):

```C++
///examples/demo_parser.cc
///
#include "demo.h"
#include <tconf_tree/iparser.h>
#include <tconf_tree/tree.h>
#include <tconf_tree/errors.h>
#include <tconf/config_reader.h>

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
```


### User defined types
To use user-defined types in your config, it's necessary to add a specialization of the struct `tconf::StringConverter` and implement its static method `fromString`.   
Let's replace the HostCfg config structure with a parameter of type Host that is stored in the config as a string `"ipAddress:port"`.

```C++
///examples/ex03.cc
///
#include <tconf/config_reader.h>
#include <tconf/shortmacros.h> //enables macros without TCONF_ prefix
#include <filesystem>
#include <iostream>
#include <vector>
#include <map>

struct Host{
    std::string ip;
    int port;
};

namespace tconf{
template<>
struct StringConverter<Host>{
    static std::optional<Host> fromString(const std::string& data)
    {
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
struct SharedAlbumCfg : public tconf::Config{
    PARAM(dir, std::filesystem::path);
    PARAM(name, std::string);
    PARAMLIST(hosts, std::vector<Host>)();
};
struct PhotoViewerCfg : public tconf::Config{
    PARAM(rootDir, std::filesystem::path);
    PARAMLIST(supportedFiles, std::vector<std::string>);
    COPY_NODELIST(sharedAlbums, std::vector<SharedAlbumCfg>)();
    using StringMap = std::map<std::string, std::string>;
    DICT(envVars, StringMap)();
};

int main()
{
    auto cfgReader = tconf::ConfigReader{};
    auto cfg = cfgReader.readYaml<PhotoViewerCfg>(R"(
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
    for (const auto& file : cfg.supportedFiles)
        std::cout << "  " << file << std::endl;

    if (!cfg.sharedAlbums.empty())
        std::cout << "Shared albums:" << std::endl;
    for (const auto& album : cfg.sharedAlbums){
        std::cout << "  Album:" << album.name << std::endl;
        std::cout << "    Hosts:" << std::endl;
        for (const auto& host : album.hosts)
            std::cout << "      " << host.ip << ":" << host.port << std::endl;
    }

    return 0;
}
```

To provide additional information in the error message of the `StringConverter`, you can use
the `tconf::ValidationError` exception:

```cpp
namespace tconf{
template<>
struct StringConverter<Host>{
    static std::optional<Host> fromString(const std::string& data)
    {
        auto delimPos = data.find(':');
        if (delimPos == std::string::npos)
            throw ValidationError{"the host parameter must be in the format 'ipAddress:port'"};
        auto host = Host{};
        host.ip = data.substr(0, delimPos);
        host.port = std::stoi(data.substr(delimPos + 1, data.size() - delimPos - 1));
        return host;
    }
};
}
```

### Validators

Processed config parameters and nodes can be validated by registering constraint-checking functions or callable objects.
The signature must be compatible with `void (const T&)` where `T` is the type of the validated config structure field.
If the option's value is invalid, the validator must throw a `tconf::ValidationError` exception:

```c++
struct Cfg : tconf::Config<>{
    TCONF_PARAM(number, int).ensure( 
        [](int paramValue){
            if (paramValue < 0)
                throw tconf::ValidationError{"value can't be negative."};
        });
};
```

Let's improve the PhotoViewer program by checking that `rootDir` path exists and `supportedFiles`  parameter list isn't empty:
```c++
///examples/ex04.cc
///
#include <tconf/config_reader.h>
#include <tconf/shortmacros.h>
#include <filesystem>
#include <iostream>
#include <vector>
#include<map>

struct NotEmpty{
    template<typename TList>
    void operator()(const TList& list)
    {
        if (!list.empty())
            throw tconf::ValidationError{"can't be empty."};
    }
};

struct PhotoViewerCfg : public tconf::Config{
    PARAM(rootDir, std::filesystem::path).ensure([](const auto& path) {
        if (!std::filesystem::exists(path))
            throw tconf::ValidationError{"a path must exist"};
    });
    PARAMLIST(supportedFiles, std::vector<std::string>).ensure<NotEmpty>();
    using StringMap = std::map<std::string, std::string>;
    DICT(envVars, StringMap)();
};

int main()
{
    try {
        auto cfgReader = tconf::ConfigReader{};
        auto cfg = cfgReader.readYaml<PhotoViewerCfg>(R"(
            rootDir: ~/Photos
            supportedFiles: []
        )");

        std::cout << "Launching PhotoViewer in directory " << cfg.rootDir << std::endl;

        if (!cfg.supportedFiles.empty())
            std::cout << "Supported files:" << std::endl;
        for (const auto& file : cfg.supportedFiles)
            std::cout << "  " << file << std::endl;

        return 0;
    }
    catch(const tconf::ConfigError& e){
        std::cout << "Config error:" << e.what();
        return 1;
    }
}
```

Now the `read` method will throw an exception if configuration provides invalid `rootDir` or `supportedFiles` parameters.


## Installation
Download and link the library from carbin:
```shell
pip install carbin
carbin install gottingen/titan-conf@some_version
```

To install the library system-wide, use the following commands:
```shell
git clone https://github.com/gottingen/titan-conf.git
cd tconf
carbin install
```

build
```shell
cmake -S . -B build
cmake --build build
cmake --install build
```

After installation, you can use the `find_package()` command to make the installed library available inside your project:
```
find_package(tconf 2.0.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE tconf::tconf)   
```


## Running tests
```
cd tconf
cmake -S . -B build -DCARBIN_BUILD_TEST=ON
cmake --build build
cd build/tests && ctest
```

## Building examples
```
cd tconf
cmake -S . -B build -DCARBIN_BUILD_EXAMPLES=ON
cmake --build build
cd build/examples
```

## License
`tconf` is licensed under the [MS-PL license](/LICENSE.md)  