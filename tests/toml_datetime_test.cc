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


#define _CRT_SECURE_NO_WARNINGS
#include "assert_exception.h"
#include "tconf/toml/datetime.h"
#include "tconf/detail/string_converter.h"
#include <ctime>
#include <iostream>

namespace test_datetime {

TEST_CASE("TestDateTime, LocalDate")
{
    const auto res = tconf::StringConverter<tconf::toml::DateTimePoint>::fromString("2018-12-23");
    REQUIRE(res.has_value());
    auto tm = std::tm{};
    tm.tm_year = 118;
    tm.tm_mon = 11;
    tm.tm_mday = 23;
    const auto t = std::mktime(&tm);
    const auto tt = std::localtime(&t);
    const auto expectedTimePoint = std::chrono::system_clock::from_time_t(std::mktime(tt));
    REQUIRE_EQ(res->value, expectedTimePoint);
}

TEST_CASE("TestDateTime, LocalDateTime")
{
    const auto res = tconf::StringConverter<tconf::toml::DateTimePoint>::fromString("2018-12-23T12:30:00");
    REQUIRE(res.has_value());
    auto tm = std::tm{};
    tm.tm_year = 118;
    tm.tm_mon = 11;
    tm.tm_mday = 23;
    tm.tm_hour = 12;
    tm.tm_min = 30;
    tm.tm_sec = 0;
    const auto t = std::mktime(&tm);
    const auto tt = std::localtime(&t);
    const auto expectedTimePoint = std::chrono::system_clock::from_time_t(std::mktime(tt));
    REQUIRE_EQ(res->value, expectedTimePoint);
}

namespace crossplatform {

std::time_t timegm(std::tm* _Tm)
{
    auto t = std::mktime(_Tm);
    return t + (std::mktime(std::localtime(&t)) - std::mktime(std::gmtime(&t)));
}

} //namespace crossplatform

TEST_CASE("TestDateTime, OffsetDateTime")
{
    const auto res = tconf::StringConverter<tconf::toml::DateTimePoint>::fromString("2018-12-23T12:30:00+09:30");
    REQUIRE(res.has_value());
    auto tm = std::tm{};
    tm.tm_year = 2018 - 1900;
    tm.tm_mon = 12 - 1;
    tm.tm_mday = 23;
    tm.tm_hour = 3;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    const auto t = crossplatform::timegm(&tm);
    const auto expectedTimePoint = std::chrono::system_clock::from_time_t(t);
    REQUIRE_EQ(res->value, expectedTimePoint);
}

TEST_CASE("TestDateTime, LocalTime")
{
    const auto res = tconf::StringConverter<tconf::toml::TimeDuration>::fromString("12:30:00");
    REQUIRE(res.has_value());
    const auto expectedTime = std::chrono::seconds{45000};
    REQUIRE_EQ(res->value, expectedTime);
}

} //namespace test_datetime