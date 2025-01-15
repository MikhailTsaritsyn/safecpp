//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_REFERENCE_IMMUTABLE_HPP
#define SAFE_REFERENCE_IMMUTABLE_HPP
#include <cassert>

#include "ReferenceCounter.hpp"

namespace safe {
/**
 * @brief Wrapper around read-only reference to a type
 *
 * @tparam T Referenced type
 */
template <typename T>
    requires(!std::is_reference_v<T>)
class ReferenceImmutable {
public:
    /**
     * @brief Construct a wrapper and increase the reference counter
     *
     * @param ref Reference to wrap
     * @param count Reference counter to track
     *
     * @note Should only be called by @link BorrowChecker @endlink
     */
    constexpr explicit ReferenceImmutable(const T &ref, internal::ReferenceCounter<size_t> &count) noexcept
            : _ref(ref),
              _count(&count) {
        _count->inc();
    }

    /**
     * @brief Copy the wrapper and increase the reference counter
     */
    constexpr ReferenceImmutable(const ReferenceImmutable &other) noexcept : _ref(other._ref), _count(other._count) {
        if (_count) _count->inc();
    }

    ReferenceImmutable &operator=(const ReferenceImmutable &other) noexcept;

    ReferenceImmutable(ReferenceImmutable &&other) noexcept : _ref(other._ref), _count(other._count) {
        other._count = nullptr;
    }

    ReferenceImmutable &operator=(ReferenceImmutable &&other) noexcept;

    /**
     * @brief Decrease the reference counter
     */
    constexpr ~ReferenceImmutable() noexcept;

    // ReSharper disable once CppNonExplicitConversionOperator
    /**
     * @brief Get read-only access to the wrapped reference
     */
    constexpr const T &operator*() const noexcept { return _ref; }

    /**
     * @brief Access methods of the underlying object
     */
    [[nodiscard]] constexpr const T *operator->() const noexcept { return &_ref; }

private:
    const T &_ref;

    /**
     * @brief Reference counter
     *
     * Can only be @p nullptr if the object was moved away.
     *
     * @note Is increased exactly once in the constructor and decreased exactly
     * once in the destructor
     */
    internal::ReferenceCounter<size_t> *_count;
};

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr ReferenceImmutable<T>::~ReferenceImmutable() noexcept {
    try {
        if (_count) _count->dec();
    } catch (std::runtime_error &) { assert("Reference count is already zero" && false); }
}

template <typename T>
    requires(!std::is_reference_v<T>)
ReferenceImmutable<T> &ReferenceImmutable<T>::operator=(const ReferenceImmutable &other) noexcept {
    if (this == &other) return *this;
    _ref   = other._ref;
    _count = other._count;
    if (_count) _count->inc();
    return *this;
}

template <typename T>
    requires(!std::is_reference_v<T>)
ReferenceImmutable<T> &ReferenceImmutable<T>::operator=(ReferenceImmutable &&other) noexcept {
    if (this == &other) return *this;
    _ref         = other._ref;
    _count       = other._count;
    other._count = nullptr;
    return *this;
}

} // namespace safe

#endif // SAFE_REFERENCE_IMMUTABLE_HPP
