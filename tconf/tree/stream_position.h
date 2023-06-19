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

#ifndef TCONF_TREE_STREAM_POSITION_H_
#define TCONF_TREE_STREAM_POSITION_H_

#include <optional>
#include <sstream>
#include <string>

namespace tconf {

    struct StreamPosition {
        std::optional<int> line;
        std::optional<int> column;
    };

    inline std::string streamPositionToString(const StreamPosition &pos) {
        auto ss = std::stringstream{};
        if (pos.line) {
            ss << "[line:" << *pos.line;
            if (pos.column)
                ss << ", column:" << *pos.column;
            ss << "] ";
        }
        return ss.str();
    }

}  // namespace tconf

#endif  // TCONF_TREE_STREAM_POSITION_H_
