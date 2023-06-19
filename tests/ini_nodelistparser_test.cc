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
#include "tconf/ini/parser.h"
#include "tconf/errors.h"


namespace test_nodelistparser {

    auto parse(const std::string &str) {
        auto input = std::stringstream{str};
        auto parser = tconf::ini::Parser{};
        return parser.parse(input);
    }

    TEST_CASE("TestNodeListParser, Basic")
    {
        auto result = parse(R"(
    testStr = Hello
    [testNodes.0]
      testInt = 3
    [testNodes.1]
      testInt = 2
    )");

        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 1);
        REQUIRE(tree.hasParam("testStr"));
        REQUIRE(tree.param("testStr").isItem());
        REQUIRE_EQ(tree.param("testStr").value(), "Hello");
        REQUIRE_EQ(tree.nodesCount(), 1);
        REQUIRE(tree.hasNode("testNodes"));
        REQUIRE(tree.node("testNodes").isList());
        auto &testNodes = tree.node("testNodes").asList();
        REQUIRE_EQ(testNodes.count(), 2);
        {
            auto &nodeData = testNodes.node(0).asItem();
            REQUIRE_EQ(nodeData.paramsCount(), 1);
            REQUIRE(nodeData.hasParam("testInt"));
            REQUIRE(nodeData.param("testInt").isItem());
            REQUIRE_EQ(nodeData.param("testInt").value(), "3");
        }
        {
            auto &nodeData = testNodes.node(1).asItem();
            REQUIRE_EQ(nodeData.paramsCount(), 1);
            REQUIRE(nodeData.hasParam("testInt"));
            REQUIRE(nodeData.param("testInt").isItem());
            REQUIRE_EQ(nodeData.param("testInt").value(), "2");
        }
    }


    TEST_CASE("TestNodeListParser- Nested")
    {
        auto result = parse(R"(
        testDouble = 0.5
        [testCfg]
            testStr = Hello
            [testCfg.testNodes.0]
                testInt = 3
            [testCfg.testNodes.1]
                testInt = 2
    )");

        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 1);
        REQUIRE(tree.hasParam("testDouble"));
        REQUIRE(tree.param("testDouble").isItem());
        REQUIRE_EQ(tree.param("testDouble").value(), "0.5");
        REQUIRE_EQ(tree.nodesCount(), 1);
        REQUIRE(tree.hasNode("testCfg"));
        auto &testCfg = tree.node("testCfg").asItem();
        REQUIRE_EQ(testCfg.paramsCount(), 1);
        REQUIRE(testCfg.hasParam("testStr"));
        REQUIRE(testCfg.param("testStr").isItem());
        REQUIRE_EQ(testCfg.param("testStr").value(), "Hello");
        REQUIRE_EQ(testCfg.nodesCount(), 1);
        REQUIRE(testCfg.hasNode("testNodes"));
        auto &testNodes = testCfg.node("testNodes").asList();
        REQUIRE_EQ(testNodes.count(), 2);
        {
            auto &nodeData = testNodes.node(0).asItem();
            REQUIRE_EQ(nodeData.paramsCount(), 1);
            REQUIRE(nodeData.hasParam("testInt"));
            REQUIRE(nodeData.param("testInt").isItem());
            REQUIRE_EQ(nodeData.param("testInt").value(), "3");
        }
        {
            auto &nodeData = testNodes.node(1).asItem();
            REQUIRE_EQ(nodeData.paramsCount(), 1);
            REQUIRE(nodeData.hasParam("testInt"));
            REQUIRE(nodeData.param("testInt").isItem());
            REQUIRE_EQ(nodeData.param("testInt").value(), "2");
        }
    }

    TEST_CASE("TestNodeListParser, Nested2")
    {
        auto result = parse(R"(
    testStr = Hello
    [testNodes.0]
      testInt = 3
    [testNodes.0.nested]
      testStr = Foo
    [testNodes.1]
      testInt = 2
    )");

        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 1);
        REQUIRE(tree.hasParam("testStr"));
        REQUIRE(tree.param("testStr").isItem());
        REQUIRE_EQ(tree.param("testStr").value(), "Hello");
        REQUIRE_EQ(tree.nodesCount(), 1);
        REQUIRE(tree.hasNode("testNodes"));
        REQUIRE(tree.node("testNodes").isList());
        auto &testNodes = tree.node("testNodes").asList();
        REQUIRE_EQ(testNodes.count(), 2);
        {
            auto &nodeData = testNodes.node(0).asItem();
            REQUIRE_EQ(nodeData.paramsCount(), 1);
            REQUIRE(nodeData.hasParam("testInt"));
            REQUIRE(nodeData.param("testInt").isItem());
            REQUIRE_EQ(nodeData.param("testInt").value(), "3");

            REQUIRE(nodeData.hasNode("nested"));
            REQUIRE(nodeData.node("nested").isItem());
            REQUIRE_EQ(nodeData.node("nested").asItem().param("testStr").value(), "Foo");
        }
        {
            auto &nodeData = testNodes.node(1).asItem();
            REQUIRE_EQ(nodeData.paramsCount(), 1);
            REQUIRE(nodeData.hasParam("testInt"));
            REQUIRE(nodeData.param("testInt").isItem());
            REQUIRE_EQ(nodeData.param("testInt").value(), "2");
        }
    }


    TEST_CASE("TestNodeListParser, NestedCfgList")
    {
        auto result = parse(R"(
        testStr = Hello
        [testList.0]
            testStr = Hello
            [testList.0.testNodes.0]
                testInt = 3
            [testList.0.testNodes.1]
                testInt = 33

        [testList.1]
            testStr = World
            [testList.1.testNodes.0]
                testInt = 5
    )");

        auto &tree = result.asItem();
        REQUIRE_EQ(tree.paramsCount(), 1);
        REQUIRE(tree.hasParam("testStr"));
        REQUIRE_EQ(tree.param("testStr").value(), "Hello");
        REQUIRE_EQ(tree.nodesCount(), 1);
        REQUIRE(tree.hasNode("testList"));
        REQUIRE(tree.node("testList").isList());
        auto &testList = tree.node("testList").asList();
        REQUIRE_EQ(testList.count(), 2);
        {
            auto &nodeData = testList.node(0).asItem();
            REQUIRE_EQ(nodeData.paramsCount(), 1);
            REQUIRE_EQ(nodeData.hasParam("testStr"), 1);
            REQUIRE_EQ(nodeData.param("testStr").value(), "Hello");
            REQUIRE_EQ(nodeData.nodesCount(), 1);
            REQUIRE_EQ(nodeData.hasNode("testNodes"), 1);
            auto &testNodes = nodeData.node("testNodes").asList();
            REQUIRE_EQ(testNodes.count(), 2);
            {
                auto &listNodeData = testNodes.node(0).asItem();
                REQUIRE_EQ(listNodeData.paramsCount(), 1);
                REQUIRE_EQ(listNodeData.hasParam("testInt"), 1);
                REQUIRE(listNodeData.param("testInt").isItem());
                REQUIRE_EQ(listNodeData.param("testInt").value(), "3");
            }
            {
                auto &listNodeData = testNodes.node(1).asItem();
                REQUIRE_EQ(listNodeData.paramsCount(), 1);
                REQUIRE_EQ(listNodeData.hasParam("testInt"), 1);
                REQUIRE(listNodeData.param("testInt").isItem());
                REQUIRE_EQ(listNodeData.param("testInt").value(), "33");
            }
        }
        {
            auto &nodeData = testList.node(1).asItem();
            REQUIRE_EQ(nodeData.paramsCount(), 1);
            REQUIRE(nodeData.hasParam("testStr"));
            REQUIRE(nodeData.param("testStr").isItem());
            REQUIRE_EQ(nodeData.param("testStr").value(), "World");
            REQUIRE_EQ(nodeData.nodesCount(), 1);
            REQUIRE(nodeData.hasNode("testNodes"));
            auto &testNodes = nodeData.node("testNodes").asList();
            REQUIRE_EQ(testNodes.count(), 1);
            {
                auto &listNodeData = testNodes.node(0).asItem();
                REQUIRE_EQ(listNodeData.paramsCount(), 1);
                REQUIRE(listNodeData.hasParam("testInt"));
                REQUIRE_EQ(listNodeData.param("testInt").value(), "5");
            }
        }
    }

}
