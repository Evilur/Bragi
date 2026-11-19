#pragma once

#include <type_traits>

template <class T>
using access_t =
    std::conditional_t<
        std::is_trivially_copyable_v<T> &&
        sizeof(T) <= 2 * sizeof(void*),
        T, const T&
    >;
