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


#ifndef TCONF_CONFIG_TREE_IPARSER_H_
#define TCONF_CONFIG_TREE_IPARSER_H_

#include "tconf/tree/tree.h"
#include <istream>

namespace tconf {
    class IParser {
    public:
        virtual ~IParser() = default;

        virtual TreeNode parse(std::istream &stream) = 0;
    };

} //namespace tconf

#endif  // TCONF_CONFIG_TREE_IPARSER_H_
