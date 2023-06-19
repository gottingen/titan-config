
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

#include "assert_exception.h"
#include "tconf/yaml/parser.h"
#include <sstream>

namespace test_paramlistparser {

    auto parseParam(const std::string &str) {
        auto input = std::stringstream{str};
        auto parser = tconf::yaml::Parser{};
        return parser.parse(input);
    }

    TEST_CASE("TestParamListParser, Basic")
    {
        auto result = parseParam(R"(
testIntList : [1, 2, 3]
)");
        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 1);
        REQUIRE_EQ(tree.param("testIntList").valueList(), (std::vector<std::string>{"1", "2", "3"}));
    }

    TEST_CASE("TestParamListParser, BasicWithoutMacro")
    {
        auto result = parseParam(R"(
testIntList : [1, 2, 3]
)");
        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 1);
        REQUIRE_EQ(tree.param("testIntList").valueList(), (std::vector<std::string>{"1", "2", "3"}));
    }

} //namespace test_paramlistparser
