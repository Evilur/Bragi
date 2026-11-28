#pragma once

#include "util/move.h"

#include <cassert>
#include <type_traits>

template <typename T>
requires (!std::is_pointer_v<T>)
class Nullable {
public:
    Nullable() noexcept = delete;
    ~Nullable() noexcept;

    Nullable(const T& value) noexcept;
    Nullable& operator=(const T& value) noexcept;
    Nullable(T&& value) noexcept;
    Nullable& operator=(T&& value) noexcept;

    Nullable(std::nullptr_t) noexcept;
    Nullable& operator=(std::nullptr_t) noexcept;

    Nullable(const Nullable& other) noexcept;
    Nullable& operator=(const Nullable& other) noexcept;
    Nullable(Nullable&& other) noexcept;
    Nullable& operator=(Nullable&& other) noexcept;

    [[nodiscard]]
    T& operator*() noexcept;
    [[nodiscard]]
    const T& operator*() const noexcept;

    [[nodiscard]]
    T* operator->() noexcept;
    [[nodiscard]]
    const T* operator->() const noexcept;

    [[nodiscard]]
    bool operator==(std::nullptr_t) const noexcept;
    [[nodiscard]]
    bool operator!=(std::nullptr_t) const noexcept;

private:
    void Construct(const T& value) noexcept;
    void Construct(T&& value) noexcept;

    void Reset() noexcept;

    [[nodiscard]]
    T* Ptr() noexcept;
    [[nodiscard]]
    const T* Ptr() const noexcept;

    alignas(T) unsigned char _storage[sizeof(T)];
    bool _has_value = false;
};


template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>::~Nullable() noexcept { Reset(); }

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>::Nullable(const T& value) noexcept { Construct(value); }

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>& Nullable<T>::operator=(const T& value)
noexcept {
    if (_has_value) *Ptr() = value;
    else Construct(value);
    return *this;
}

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>::Nullable(T&& value) noexcept { Construct(move(value)); }

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>& Nullable<T>::operator=(T&& value)
noexcept {
    if (_has_value) *Ptr() = move(value);
    else Construct(move(value));
    return *this;
}

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>::Nullable(std::nullptr_t) noexcept { }

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>& Nullable<T>::operator=(std::nullptr_t)
noexcept { Reset(); return *this; }

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>::Nullable(const Nullable& other)
noexcept { if (other._has_value) Construct(*other); }

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>& Nullable<T>::operator=(const Nullable& other)
noexcept {
    if (this == &other) return *this;
    if (_has_value) {
        if (other._has_value)
            *Ptr() = *other;
        else
            Reset();
    } else {
        if (other._has_value)
            Construct(*other);
    }
    return *this;
}

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>::Nullable(Nullable&& other)
noexcept {
    if (other._has_value) {
        Construct(move(*other));
        other.Reset();
    }
}

template <typename T>
requires (!std::is_pointer_v<T>)
inline Nullable<T>& Nullable<T>::operator=(Nullable&& other)
noexcept {
    if (this == &other) return *this;
    if (_has_value) {
        if (other._has_value) {
            *Ptr() = move(*other);
            other.Reset();
        } else Reset();
    } else {
        if (other._has_value) {
            Construct(move(*other));
            other.Reset();
        }
    }
    return *this;
}

template <typename T>
requires (!std::is_pointer_v<T>)
inline T& Nullable<T>::operator*() noexcept {
    assert(_has_value && "Trying to get the value of null");
    return *Ptr();
}

template <typename T>
requires (!std::is_pointer_v<T>)
inline const T& Nullable<T>::operator*() const noexcept {
    assert(_has_value && "Trying to get the value of null");
    return *Ptr();
}

template <typename T>
requires (!std::is_pointer_v<T>)
inline T* Nullable<T>::operator->() noexcept {
    assert(_has_value && "Trying to access the null pointer");
    return Ptr();
}

template <typename T>
requires (!std::is_pointer_v<T>)
inline const T* Nullable<T>::operator->() const noexcept {
    assert(_has_value && "Trying to access the null pointer");
    return Ptr();
}

template <typename T>
requires (!std::is_pointer_v<T>)
inline bool Nullable<T>::operator==(std::nullptr_t) const
noexcept { return !_has_value; }

template <typename T>
requires (!std::is_pointer_v<T>)
inline bool Nullable<T>::operator!=(std::nullptr_t) const
noexcept { return _has_value; }

template <typename T>
requires (!std::is_pointer_v<T>)
inline void Nullable<T>::Construct(const T& value)
noexcept { new (_storage) T(value); _has_value = true; }

template <typename T>
requires (!std::is_pointer_v<T>)
inline void Nullable<T>::Construct(T&& value)
noexcept { new (_storage) T(move(value)); _has_value = true; }

template <typename T>
requires (!std::is_pointer_v<T>)
inline void Nullable<T>::Reset()
noexcept { if (_has_value) { Ptr()->~T(); _has_value = false; } }

template <typename T>
requires (!std::is_pointer_v<T>)
inline T* Nullable<T>::Ptr()
noexcept { return (T*)(_storage); }

template <typename T>
requires (!std::is_pointer_v<T>)
inline const T* Nullable<T>::Ptr() const
noexcept { return (const T*)(_storage); }
