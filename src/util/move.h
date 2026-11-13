#pragma once

template<typename T>
consteval T&& move(T& obj) noexcept;

template<typename T>
inline consteval T&& move(T& obj) noexcept { return (T&&)obj; }
