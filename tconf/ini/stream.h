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

#include "tconf/tree/stream_position.h"
#include <istream>
#include <string>

namespace tconf::ini::detail {

    class Stream {
    public:
        explicit Stream(std::istream &stream, const StreamPosition &startPosition = StreamPosition{1, 1});

        Stream(Stream &&) = default;

        Stream(const Stream &) = delete;

        Stream &operator=(const Stream &) = delete;

        void skip(int size);

        std::string read(int size = 1);

        std::string peek(int size = 1);

        bool atEnd();

        StreamPosition position() const;

    private:
        void skipLine();

    private:
        std::istream &stream_;
        StreamPosition position_ = {0, 0};
        StreamPosition startPosition_ = {0, 0};
    };

} //namespace tconf::ini::detail
