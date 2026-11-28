#pragma once

#include "util/class.h"

#include <cstdint>

/**
 * A simple ring queue implementation
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T, uint32_t N>
class Queue final {
public:
    static_assert(N > 0, "The queue can not be zero-length");

    PREVENT_COPY_ALLOW_MOVE(Queue);

    Queue() noexcept = default;

    constexpr void Push(const T& value) noexcept;

    constexpr void Push(T&& value) noexcept;

    [[nodiscard]]
    constexpr T Pop() noexcept;

    constexpr void Clear() noexcept;

    [[nodiscard]]
    constexpr uint32_t Size() const noexcept;

    [[nodiscard]]
    constexpr bool IsEmpty() const noexcept;

    [[nodiscard]]
    constexpr bool IsFull() const noexcept;

private:
    T _data[N] { };
    uint32_t _head = 0;
    uint32_t _tail = 0;
    uint32_t _size = 0;
};

template <typename T, uint32_t N>
inline constexpr void Queue<T, N>::Push(const T& value) noexcept {
    assert(!IsFull());

    /* Set the new value */
    _data[_tail] = value;

    /* Increase the size */
    ++_size;

    /* Update the tail */
    ++_tail;
    if (_tail == N) _tail = 0;
}

template <typename T, uint32_t N>
inline constexpr void Queue<T, N>::Push(T&& value) noexcept {
    assert(!IsFull() && "There is no free space in the queue to push");

    /* Set the new value */
    _data[_tail] = move(value);

    /* Increase the size */
    ++_size;

    /* Update the tail */
    ++_tail;
    if (_tail == N) _tail = 0;
}

template <typename T, uint32_t N>
inline constexpr T Queue<T, N>::Pop() noexcept {
    assert(!IsEmpty() && "The queue has no elements to pop");

    /* Get the value from the buffer */
    T value = move(_data[_head]);

    /* Decrease the size */
    --_size;

    /* Update the head */
    ++_head;
    if (_head == N) _head = 0;

    /* Return the value */
    return value;
}

template <typename T, uint32_t N>
inline constexpr void Queue<T, N>::Clear() noexcept {
    /* Reset the size */
    _size = 0;

    /* Reset the head and the tail */
    _head = 0;
    _tail = 0;
}

template <typename T, uint32_t N>
inline constexpr uint32_t Queue<T, N>::Size() const noexcept { return _size; }

template <typename T, uint32_t N>
inline constexpr bool Queue<T, N>::IsEmpty() const
noexcept { return _size == 0; }

template <typename T, uint32_t N>
inline constexpr bool Queue<T, N>::IsFull() const
noexcept { return _size == N; }
