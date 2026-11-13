#pragma once

#include "util/class.h"
#include "util/move.h"

#include <algorithm>
#include <cstdint>

/**
 * A simple implementation of a dynamic array with variable length
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T, std::unsigned_integral S = uint32_t>
class DynamicArray final {
public:
    DynamicArray();
    explicit DynamicArray(S capacity);

    DynamicArray(const DynamicArray & other);
    DynamicArray &operator=(const DynamicArray & other);
    DynamicArray(DynamicArray && other) noexcept;
    DynamicArray &operator=(DynamicArray && other) noexcept;

    ~DynamicArray() noexcept;

    S Size() const noexcept;

    void Push(const T& element);
    void Push(T&& element);

    T& operator[](S index) noexcept;
    const T& operator[](S index) const noexcept;

private:
    S _capacity;
    S _size = 0;
    T* _array;

    void Grow();
};

template <typename T, std::unsigned_integral S>
inline DynamicArray<T, S>::DynamicArray()
    : _capacity(8), _array(new T[8]) { }

template <typename T, std::unsigned_integral S>
inline DynamicArray<T, S>::DynamicArray(const S capacity)
    : _capacity(capacity), _array(new T[capacity]) { }

template <typename T, std::unsigned_integral S>
inline DynamicArray<T, S>::DynamicArray(const DynamicArray& other)
    : _capacity(other._capacity),
    _size(other._size),
    _array(new T[other._capacity]) {
    /* Copy copy elements to the array */
    std::copy(other._array, other._array + other._size, _array);
}

template <typename T, std::unsigned_integral S>
inline DynamicArray<T, S>&
DynamicArray<T, S>::operator=(const DynamicArray& other) {
    /* Check for the self-assignment */
    if (this == &other) return *this;

    /* Allocate new memory and copy elements to the array */
    delete[] _array;
    _array = new T[other._capacity];
    std::copy(other._array, other._array + other._size, _array);

    /* Copy the other values */
    _capacity = other._capacity;
    _size = other._size;

    /* Return *this */
    return *this;
}

template <typename T, std::unsigned_integral S>
inline DynamicArray<T, S>::DynamicArray(DynamicArray&& other) noexcept
    : _capacity(other._capacity),
    _size(other._size),
    _array(other._array) {
    /* Null the other's values */
    other._capacity = 0;
    other._size = 0;
    other._array = nullptr;
}

template <typename T, std::unsigned_integral S>
inline DynamicArray<T, S>&
DynamicArray<T, S>::operator=(DynamicArray&& other) noexcept {
    /* Check for the self-assignment */
    if (this == &other) return *this;

    /* Free the original memory */
    delete[] _array;

    /* Move the values from the other object */
    _capacity = other._capacity;
    _size = other._size;
    _array = other._array;

    /* Null the other's values */
    other._capacity = 0;
    other._size = 0;
    other._array = nullptr;

    /* Return *this */
    return *this;
}

template <typename T, std::unsigned_integral S>
inline DynamicArray<T, S>::~DynamicArray() noexcept { delete[] _array; }

template <typename T, std::unsigned_integral S>
inline S DynamicArray<T, S>::Size() const noexcept { return _size; }

template <typename T, std::unsigned_integral S>
inline void DynamicArray<T, S>::Push(const T& element) {
    /* Check for a free space */
    if (_size == _capacity) Grow();

    /* Push the element to the array and increase the size */
    _array[_size] = element;
    ++_size;
}

template <typename T, std::unsigned_integral S>
inline void DynamicArray<T, S>::Push(T&& element) {
    /* Check for a free space */
    if (_size == _capacity) Grow();

    /* Push the element to the array and increase the size */
    _array[_size] = move(element);
    ++_size;
}

template <typename T, std::unsigned_integral S>
inline T& DynamicArray<T, S>::operator[](const S index)
noexcept { return _array[index]; }

template <typename T, std::unsigned_integral S>
inline const T& DynamicArray<T, S>::operator[](const S index) const
noexcept { return _array[index]; }

template <typename T, std::unsigned_integral S>
inline void DynamicArray<T, S>::Grow() {
    /* Calculate the new capacity */
    const S new_capacity = _capacity < 1 ? 8 : _capacity * 2;

    /* Create a new array with the new capacity, and update the capacity */
    T* new_array = new T[new_capacity];
    _capacity = new_capacity;

    /* Copy the old array to the new one */
    std::move(_array, _array + _size, new_array);

    /* Free the memory and replace the old array with the new one */
    delete[] _array;
    _array = new_array;
}
