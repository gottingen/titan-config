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


#ifndef TCONF_SFUN_UTILITY_H
#define TCONF_SFUN_UTILITY_H

#include <cstddef>
#include <exception>
#include <type_traits>
#include <utility>

namespace tconf::sfun {

    [[noreturn]] inline void unreachable() noexcept {
        std::terminate();
    }

    template<typename T>
    inline constexpr auto dependent_false = false;

    using ssize_t = std::ptrdiff_t;
    using index_t = std::ptrdiff_t;

    template<class C>
    constexpr auto ssize(const C &c) -> std::common_type_t<ssize_t, std::make_signed_t<decltype(c.size())>> {
        using R = std::common_type_t<ssize_t, std::make_signed_t<decltype(c.size())>>;
        return static_cast<R>(c.size());
    }

    template<class T, std::ptrdiff_t N>
    constexpr ssize_t ssize(const T (&)[N]) noexcept {
        return N;
    }

    namespace detail {
        template<std::size_t N, std::size_t... Ints>
        constexpr auto shiftSequence(std::index_sequence<Ints...>) -> std::index_sequence<N + Ints...>;
    }

    template<std::size_t Begin, std::size_t End>
    using make_index_range = decltype(detail::shiftSequence<Begin>(std::make_index_sequence<End - Begin>()));

} //namespace tconf::sfun

#endif //TCONF_SFUN_UTILITY_H