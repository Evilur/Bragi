#pragma once

#include "util/move.h"

#include <type_traits>
#include <utility>

template <typename T, typename U, typename>
constexpr std::common_type_t<T, U> min(T&& x1, U&& x2);

template <typename T, typename U, typename>
constexpr std::common_type_t<T, U> max(T&& x1, U&& x2);

template <typename T, typename U,
typename = std::enable_if_t<
!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
constexpr inline std::common_type_t<T, U> min(T&& x1, U&& x2) {
    std::common_type_t<T, U> a(std::forward<T>(x1));
    std::common_type_t<T, U> b(std::forward<U>(x2));
    return (b < a) ? move(b) : move(a);
}

template <typename T, typename U,
typename = std::enable_if_t<
!std::is_same_v<std::decay_t<T>, std::decay_t<U>>>>
constexpr inline std::common_type_t<T, U> max(T&& x1, U&& x2) {
    std::common_type_t<T, U> a(std::forward<T>(x1));
    std::common_type_t<T, U> b(std::forward<U>(x2));
    return (a < b) ? move(b) : move(a);
}
