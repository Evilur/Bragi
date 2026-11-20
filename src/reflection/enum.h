#pragma once

#include <cstdint>
#include <meta>
#include <type_traits>

template <typename T>
requires std::is_enum_v<T>
constexpr const char* get_enum_label(T value) noexcept;

template <typename T>
requires std::is_enum_v<T>
constexpr uint32_t get_enum_label_length(T value) noexcept;

template <typename T>
requires std::is_enum_v<T>
inline constexpr const char* get_enum_label(T value) noexcept {
    template
    for (constexpr auto element :
         std::define_static_array(std::meta::enumerators_of(^^T)))
        if (value == [:element:])
            return
                std::define_static_string(std::meta::identifier_of(element));
    return "Undefined";
}

template <typename T>
requires std::is_enum_v<T>
inline constexpr uint32_t get_enum_label_length(T value) noexcept {
    template
    for (constexpr auto element :
         std::define_static_array(std::meta::enumerators_of(^^T)))
        if (value == [:element:])
            return std::meta::identifier_of(element).size();
    return 0;

}
