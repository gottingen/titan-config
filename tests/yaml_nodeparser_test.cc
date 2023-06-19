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
#include "tconf/errors.h"
#include "tconf/yaml/parser.h"

#include <sstream>

namespace test_nodeparser {

    auto parse(const std::string &str) {
        auto input = std::stringstream{str};
        auto parser = tconf::yaml::Parser{};
        return parser.parse(input);
    }

    TEST_CASE("TestNodeParser, SingleNode")
    {
        auto result = parse(R"(
foo: 5
bar: "test"
a:
  testInt: 10
    )");

        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 2);
        REQUIRE_EQ(tree.hasParam("foo"), 1);
        REQUIRE_EQ(tree.hasParam("bar"), 1);
        REQUIRE_EQ(tree.param("foo").value(), "5");
        REQUIRE_EQ(tree.param("bar").value(), "test");
        REQUIRE_EQ(tree.nodesCount(), 1);
        REQUIRE_EQ(tree.hasNode("a"), 1);
        auto &aNode = tree.node("a").asItem();
        REQUIRE_EQ(aNode.paramsCount(), 1);
        REQUIRE_EQ(aNode.param("testInt").value(), "10");
    }

    TEST_CASE("TestNodeParser, SingleNodeOneDocument")
    {
        auto result = parse(R"(---
foo: 5
bar: "test"
a:
  testInt: 10
    )");

        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 2);
        REQUIRE_EQ(tree.hasParam("foo"), 1);
        REQUIRE_EQ(tree.hasParam("bar"), 1);
        REQUIRE_EQ(tree.param("foo").value(), "5");
        REQUIRE_EQ(tree.param("bar").value(), "test");
        REQUIRE_EQ(tree.nodesCount(), 1);
        REQUIRE_EQ(tree.hasNode("a"), 1);
        auto &aNode = tree.node("a").asItem();
        REQUIRE_EQ(aNode.paramsCount(), 1);
        REQUIRE_EQ(aNode.param("testInt").value(), "10");
    }

    TEST_CASE("TestNodeParser, MultiNodeSingleLevel")
    {

        auto result = parse(R"(
foo: 5
bar: test
a:
  testInt : 10
b:
  testInt : 11
    )");

        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 2);
        REQUIRE_EQ(tree.hasParam("foo"), 1);
        REQUIRE_EQ(tree.hasParam("bar"), 1);
        REQUIRE_EQ(tree.param("foo").value(), "5");
        REQUIRE_EQ(tree.param("bar").value(), "test");
        REQUIRE_EQ(tree.nodesCount(), 2);
        REQUIRE_EQ(tree.hasNode("a"), 1);
        REQUIRE_EQ(tree.hasNode("b"), 1);
        auto &aNode = tree.node("a").asItem();
        REQUIRE_EQ(aNode.paramsCount(), 1);
        REQUIRE_EQ(aNode.param("testInt").value(), "10");
        auto &bNode = tree.node("b").asItem();
        REQUIRE_EQ(bNode.paramsCount(), 1);
        REQUIRE_EQ(bNode.param("testInt").value(), "11");
    }

    TEST_CASE("TestNodeParser, MultiLevel")
    {
        auto result = parse(R"(
foo : 5
bar : test
c:
  testInt : 11
  testDouble : 12
  b:
    testInt : 10
    testString : Hello world
b:
  testInt: 9
)");

        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 2);
        REQUIRE_EQ(tree.hasParam("foo"), 1);
        REQUIRE_EQ(tree.hasParam("bar"), 1);
        REQUIRE_EQ(tree.param("foo").value(), "5");
        REQUIRE_EQ(tree.param("bar").value(), "test");
        REQUIRE_EQ(tree.nodesCount(), 2);
        REQUIRE_EQ(tree.hasNode("c"), 1);
        REQUIRE_EQ(tree.hasNode("b"), 1);

        auto &cNode = tree.node("c").asItem();
        REQUIRE_EQ(cNode.paramsCount(), 2);
        REQUIRE_EQ(cNode.param("testInt").value(), "11");
        REQUIRE_EQ(cNode.param("testDouble").value(), "12");
        REQUIRE_EQ(cNode.nodesCount(), 1);
        REQUIRE_EQ(cNode.hasNode("b"), 1);
        auto &cbNode = cNode.node("b").asItem();
        REQUIRE_EQ(cbNode.paramsCount(), 2);
        REQUIRE_EQ(cbNode.param("testInt").value(), "10");
        REQUIRE_EQ(cbNode.param("testString").value(), "Hello world");

        auto &bNode = tree.node("b").asItem();
        REQUIRE_EQ(bNode.paramsCount(), 1);
        REQUIRE_EQ(bNode.param("testInt").value(), "9");
    }

} //namespace test_nodeparser