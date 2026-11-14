#pragma once

template<typename T>
T&& move(T& obj) noexcept;

template<typename T>
inline T&& move(T& obj) noexcept { return (T&&)obj; }
