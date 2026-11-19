#pragma once

template<typename T>
constexpr T&& move(T& obj) noexcept;

template<typename T>
constexpr inline T&& move(T& obj) noexcept { return (T&&)obj; }
