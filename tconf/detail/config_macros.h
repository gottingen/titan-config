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


#pragma once
#include "tconf/detail/dict_creator.h"
#include "tconf/detail/node_creator.h"
#include "tconf/detail/nodelist_creator.h"
#include "tconf/detail/param_creator.h"
#include "tconf/detail/paramlist_creator.h"

#define TCONF_PARAM(name, type) type name = param<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define TCONF_NODE(name, type) type name = node<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define TCONF_COPY_NODE_LIST(name, listType)                                                                          \
    listType name = copyNodeList<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define TCONF_NODE_LIST(name, listType) listType name = nodeList<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define TCONF_PARAM_LIST(name, listType) listType name = paramList<&std::remove_pointer_t<decltype(this)>::name>(#name)
#define TCONF_DICT(name, mapType) mapType name = dict<&std::remove_pointer_t<decltype(this)>::name>(#name)