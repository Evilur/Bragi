#pragma once

#include <cstdint>
#include <type_traits>

/**
 * Simple implementation of the smart unique pointer
 * @author Evilur <the.evilur@gmail.com>
 */
template <typename T>
class UniqPtr final {
public:
    using BaseType = std::remove_extent_t<T>;

    UniqPtr(BaseType* ptr) noexcept;

    UniqPtr(UniqPtr&& other) noexcept;

    UniqPtr& operator=(UniqPtr&& other) noexcept;

    UniqPtr(const UniqPtr&) = delete;

    UniqPtr& operator=(const UniqPtr&) = delete;

    ~UniqPtr() noexcept;

    BaseType* Get() noexcept;

    const BaseType* Get() const noexcept;

    void Release() noexcept;

    BaseType& operator*() noexcept;

    const BaseType& operator*() const noexcept;

    operator BaseType*() noexcept;

    operator const BaseType*() const noexcept;

    BaseType* operator->() noexcept
    requires (!std::is_array_v<T>);

    const BaseType* operator->() const noexcept
    requires (!std::is_array_v<T>);

    BaseType& operator[](uint64_t index) noexcept
    requires (std::is_array_v<T>);

    const BaseType& operator[](uint64_t index) const noexcept
    requires (std::is_array_v<T>);

private:
    BaseType* _ptr = nullptr;
};

template<typename T>
inline UniqPtr<T>::UniqPtr(BaseType* const ptr) noexcept
    : _ptr(ptr) { }

template<typename T>
inline UniqPtr<T>::UniqPtr(UniqPtr&& other)
noexcept : _ptr(other._ptr) {
    /* Null the other pointer */
    other._ptr = nullptr;
}

template<typename T>
inline UniqPtr<T>& UniqPtr<T>::operator=(UniqPtr&& other)
noexcept {
    /* If there is the same objecto nothing */
    if (this == &other) return *this;

    /* Free the memory */
    if constexpr (std::is_array_v<T>) delete[] _ptr;
    else delete _ptr;

    /* Swap the pointers */
    _ptr = other._ptr;

    /* Null the other pointer */
    other._ptr = nullptr;
    return *this;
}

template<typename T>
inline UniqPtr<T>::~UniqPtr() noexcept {
    if constexpr (std::is_array_v<T>) delete[] _ptr;
    else delete _ptr;
}

template<typename T>
inline UniqPtr<T>::BaseType* UniqPtr<T>::Get() noexcept { return _ptr; }

template<typename T>
inline const UniqPtr<T>::BaseType* UniqPtr<T>::Get() const
noexcept { return _ptr; }

template<typename T>
inline void UniqPtr<T>::Release() noexcept { _ptr = nullptr; }

template<typename T>
inline UniqPtr<T>::BaseType& UniqPtr<T>::operator*() noexcept { return *_ptr; }

template<typename T>
inline const UniqPtr<T>::BaseType& UniqPtr<T>::operator*() const
noexcept { return *_ptr; }

template<typename T>
inline UniqPtr<T>::operator UniqPtr<T>::BaseType*() noexcept { return _ptr; }

template<typename T>
inline UniqPtr<T>::operator const UniqPtr<T>::BaseType*() const
noexcept { return _ptr; }

template <typename T>
inline UniqPtr<T>::BaseType* UniqPtr<T>::operator->() noexcept
requires (!std::is_array_v<T>) { return _ptr; }

template <typename T>
inline const UniqPtr<T>::BaseType* UniqPtr<T>::operator->() const noexcept
requires (!std::is_array_v<T>) { return _ptr; }

template <typename T>
inline UniqPtr<T>::BaseType&
UniqPtr<T>::operator[](const uint64_t index) noexcept
requires (std::is_array_v<T>) { return _ptr[index]; }

template <typename T>
inline const UniqPtr<T>::BaseType&
UniqPtr<T>::operator[](const uint64_t index) const noexcept
requires (std::is_array_v<T>) { return _ptr[index]; }
