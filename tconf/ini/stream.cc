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
#include "tconf/ini/stream.h"

namespace tconf::ini::detail {

    Stream::Stream(std::istream &stream, const StreamPosition &startPosition)
            : stream_(stream), startPosition_(startPosition) {
    }

    void Stream::skip(int size) {
        read(size);
    }

    std::string Stream::read(int size) {
        auto result = std::string{};
        auto ch = char{};
        for (auto i = 0; i < size; ++i) {
            if (!stream_.get(ch))
                return {};
            if (ch == '\n') {
                (*position_.line)++;
                (*position_.column) = 0;
            } else if (ch == '\t')
                (*position_.column) += 4;
            else
                (*position_.column)++;
            result.push_back(ch);
        }
        return result;
    }

    std::string Stream::peek(int size) {
        auto result = std::string{};
        auto ch = char{};
        auto pos = stream_.tellg();
        for (auto i = 0; i < size; ++i) {
            if (!stream_.get(ch)) {
                stream_.clear();
                result.clear();
                break;
            }
            result.push_back(ch);
        }
        stream_.seekg(pos);
        return result;
    }

    bool Stream::atEnd() {
        return peek().empty();
    }

    StreamPosition Stream::position() const {
        return {*startPosition_.line + *position_.line, *startPosition_.column + *position_.column};
    }

    void Stream::skipLine() {
        auto ch = char{};
        auto prevPos = stream_.tellg();
        while (stream_.get(ch)) {
            if (ch == '\n') {
                stream_.seekg(prevPos);
                break;
            }
            prevPos = stream_.tellg();
        }
    }

} //namespace tconf::ini::detail