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


#ifndef TCONF_SFUN_FUNCTIONAL_H
#define TCONF_SFUN_FUNCTIONAL_H

#include "tconf/detail/type_list.h"
#include <utility>

namespace tconf::sfun {

    namespace detail {

        template<typename>
        struct function_guide_helper {
        };

        template<typename Res, typename Tp, typename... Args>
        struct function_guide_helper<Res (Tp::*)(Args...)> {
            using type = Res(Args...);
        };

        template<typename R, typename Tp, typename... Args>
        struct function_guide_helper<R (Tp::*)(Args...) &> {
            using type = R(Args...);
        };

        template<typename R, typename Tp, typename... Args>
        struct function_guide_helper<R (Tp::*)(Args...) const> {
            using type = R(Args...);
        };

        template<typename R, typename Tp, typename... Args>
        struct function_guide_helper<R (Tp::*)(Args...) const &> {
            using type = R(Args...);
        };

        template<typename Res, typename Tp, typename... Args>
        struct function_guide_helper<Res (Tp::*)(Args...) noexcept> {
            using type = Res(Args...);
        };

        template<typename R, typename Tp, typename... Args>
        struct function_guide_helper<R (Tp::*)(Args...) &noexcept> {
            using type = R(Args...);
        };

        template<typename R, typename Tp, typename... Args>
        struct function_guide_helper<R (Tp::*)(Args...) const noexcept> {
            using type = R(Args...);
        };

        template<typename R, typename Tp, typename... Args>
        struct function_guide_helper<R (Tp::*)(Args...) const &noexcept> {
            using type = R(Args...);
        };

        template<typename Signature>
        struct callable_storage;

        template<typename R, typename... Args>
        struct callable_storage<R(Args...)> {
            template<typename TFunctor>
            callable_storage(TFunctor &&) {
            }
        };

        template<class R, class... ArgTypes>
        callable_storage(R (*)(ArgTypes...)) -> callable_storage<R(ArgTypes...)>;

        template<typename Functor, typename Signature = typename function_guide_helper<decltype(&Functor::operator())>::type>
        callable_storage(Functor &&) -> callable_storage<Signature>;

        template<typename Functor, typename Signature = typename function_guide_helper<decltype(&Functor::operator())>::type>
        callable_storage(Functor &) -> callable_storage<Signature>;

    } //namespace detail

    template<typename T>
    struct callable_signature;

    template<typename R, typename... Args>
    struct callable_signature<detail::callable_storage<R(Args...)>> {
        using return_type = R;
        using args = type_list<Args...>;
    };

    template<typename TCallable>
    using callable_args = typename callable_signature<decltype(detail::callable_storage{
            std::declval<TCallable>()})>::args;

    template<typename TCallable>
    using callable_return_type =
            typename callable_signature<decltype(detail::callable_storage{std::declval<TCallable>()})>::return_type;

    template<class... Ts>
    struct overloaded : Ts ... {
        using Ts::operator()...;
    };
    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

} //namespace tconf::sfun

#endif //TCONF_SFUN_FUNCTIONAL_H
