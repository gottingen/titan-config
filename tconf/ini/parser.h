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

#ifndef TCONF_INI_INI_PARSER_H_
#define TCONF_INI_INI_PARSER_H_

#include "tconf/tree/iparser.h"
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace tconf::ini {

    class Parser : public IParser {
        class Impl;

    public:
        Parser();

        TreeNode parse(std::istream &stream) override;

        ~Parser() override;

        Parser(const Parser &) = delete;

        Parser(Parser &&) = delete;

        Parser &operator=(const Parser &) = delete;

        Parser &operator=(Parser &&) = delete;

    private:
        std::unique_ptr<Impl> impl_;
    };

}  // namespace tconf::ini

#endif  // TCONF_INI_INI_PARSER_H_