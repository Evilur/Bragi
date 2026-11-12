#pragma once

#include "util/macro.h"

#include <cstdint>

template <typename T>
inline void delete_object(T* obj) noexcept;

template <typename T>
inline void delete_array(T* arr) noexcept;

template <typename T>
using deleter = void(*)(T*);

/**
 * Simple implementation of the smart unique pointer
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T, deleter<T> D>
class UniqPtrBase {
public:
    UniqPtrBase(T* ptr) noexcept;

    UniqPtrBase(UniqPtrBase&& other) noexcept;

    UniqPtrBase& operator=(UniqPtrBase&& other) noexcept;

    UniqPtrBase(const UniqPtrBase&) noexcept = delete;

    UniqPtrBase& operator=(const UniqPtrBase&) noexcept = delete;

    ~UniqPtrBase() noexcept;

    T* Get() noexcept;

    const T* Get() const noexcept;

    void Release() noexcept;

    T& operator*() noexcept;

    const T& operator*() const noexcept;

    operator T*() noexcept;

    operator const T*() const noexcept;

protected:
    T* _ptr = nullptr;
};

template <typename T>
class UniqPtr final : public UniqPtrBase<T, delete_object> {
public:
    using UniqPtrBase<T, delete_object>::UniqPtrBase;

    T* operator->() noexcept;

    const T* operator->() const noexcept;
};

template <typename T>
class UniqPtr<T[]> final : public UniqPtrBase<T, delete_array> {
public:
    using UniqPtrBase<T, delete_array>::UniqPtrBase;

    T& operator[](uint64_t index) noexcept;

    const T& operator[](uint64_t index) const noexcept;
};

template<typename T>
FORCE_INLINE void delete_object(T* const obj) noexcept { delete obj; }

template<typename T>
FORCE_INLINE void delete_array(T* const arr) noexcept { delete[] arr; }

template<typename T, deleter<T> D>
FORCE_INLINE
UniqPtrBase<T, D>::UniqPtrBase(T* const ptr) noexcept : _ptr(ptr) { }

template<typename T, deleter<T> D>
FORCE_INLINE
UniqPtrBase<T, D>::UniqPtrBase(UniqPtrBase&& other)
noexcept : _ptr(other._ptr) {
    /* Null the other pointer */
    other._ptr = nullptr;
}

template<typename T, deleter<T> D>
FORCE_INLINE
UniqPtrBase<T, D>& UniqPtrBase<T, D>::operator=(UniqPtrBase&& other) noexcept {
    /* If there is the same object, do nothing */
    if (this == &other) return *this;

    /* Free the memory */
    (D(_ptr));

    /* Swap the pointers */
    _ptr = other._ptr;

    /* Null the other pointer */
    other._ptr = nullptr;
    return *this;
}

template<typename T, deleter<T> D>
FORCE_INLINE
UniqPtrBase<T, D>::~UniqPtrBase() noexcept { (D(_ptr)); }

template<typename T, deleter<T> D>
FORCE_INLINE
T* UniqPtrBase<T, D>::Get() noexcept { return _ptr; }

template<typename T, deleter<T> D>
FORCE_INLINE
const T* UniqPtrBase<T, D>::Get() const noexcept { return _ptr; }

template<typename T, deleter<T> D>
FORCE_INLINE
void UniqPtrBase<T, D>::Release() noexcept { _ptr = nullptr; }

template<typename T, deleter<T> D>
FORCE_INLINE
T& UniqPtrBase<T, D>::operator*() noexcept { return *_ptr; }

template<typename T, deleter<T> D>
FORCE_INLINE
const T& UniqPtrBase<T, D>::operator*() const noexcept { return *_ptr; }

template<typename T, deleter<T> D>
FORCE_INLINE
UniqPtrBase<T, D>::operator T*() noexcept { return _ptr; }

template<typename T, deleter<T> D>
FORCE_INLINE
UniqPtrBase<T, D>::operator const T*() const noexcept { return _ptr; }

template<typename T>
FORCE_INLINE
T* UniqPtr<T>::operator->() noexcept { return this->_ptr; }

template<typename T>
FORCE_INLINE
const T* UniqPtr<T>::operator->() const noexcept { return this->_ptr; }

template<typename T>
FORCE_INLINE
T& UniqPtr<T[]>::operator[](const uint64_t index) noexcept {
    return this->_ptr[index];
}

template<typename T>
FORCE_INLINE
const T& UniqPtr<T[]>::operator[](const uint64_t index) const noexcept {
    return this->_ptr[index];
}
