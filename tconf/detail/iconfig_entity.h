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

#include "tconf/detail/interface.h"
#include <tconf/tree/stream_position.h>
#include <functional>
#include <string>

namespace tconf::detail {

    class IConfigEntity : private sfun::interface<IConfigEntity> {
    public:
        virtual StreamPosition position() = 0;

        virtual std::string description() = 0;
    };

} //namespace tconf::detail