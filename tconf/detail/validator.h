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

#include "tconf/detail/iconfig_entity.h"
#include "tconf/detail/ivalidator.h"
#include <tconf/errors.h>
#include <functional>

namespace tconf::detail {

    template<typename T>
    class Validator : public IValidator {
    public:
        Validator(IConfigEntity &entity, T &entityValue, std::function<void(const T &)> validatingFunc)
                : entity_(entity), entityValue_(entityValue), validatingFunc_(std::move(validatingFunc)) {
        }

    private:
        void validate() override {
            try {
                validatingFunc_(entityValue_);
            }
            catch (const ValidationError &e) {
                throw ConfigError{entity_.description() + ": " + e.what(), entity_.position()};
            }
        }

        IConfigEntity &entity_;
        T &entityValue_;
        std::function<void(const T &)> validatingFunc_;
    };

} //namespace tconf::detail