//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_REFERENCE_MUTABLE_HPP
#define SAFE_REFERENCE_MUTABLE_HPP
#include <cassert>

#include "ReferenceLock.hpp"

namespace safe {
/**
 * @brief Wrapper around read-write reference to a value
 *
 * @tparam T Referenced type
 */
template <typename T>
    requires(!std::is_reference_v<T>)
class ReferenceMutable {
public:
    /**
     * @brief Construct the wrapper and acquire access to it
     *
     * @param ref Reference to borrow
     * @param lock Modifiable reference counter
     *
     * @note Should only be called by @link BorrowChecker @endlink
     *
     * @throws std::runtime_error If the lock is already acquired
     */
    constexpr explicit ReferenceMutable(T &ref, internal::ReferenceLock &lock) : _ref(ref), _lock(lock) {
        _lock.lock();
    }

    ReferenceMutable(const ReferenceMutable &) noexcept            = delete;
    ReferenceMutable &operator=(const ReferenceMutable &) noexcept = delete;

    ReferenceMutable(ReferenceMutable &&) noexcept            = default;
    ReferenceMutable &operator=(ReferenceMutable &&) noexcept = default;

    /**
     * @brief Releases the wrapped reference allowing to borrow it again
     */
    constexpr ~ReferenceMutable() noexcept {
        try {
            _lock.unlock();
        } catch (std::runtime_error &) { assert("Lock has been reset" && false); }
    }

    /**
     * @brief Update the referenced object
     *
     * Given value is copied or moved to the referenced object.
     *
     * @throws ... Whatever exceptions are generated by the assignment of the
     *             referenced object
     */
    constexpr ReferenceMutable &operator=(T &&value) {
        _ref = std::forward<T>(value);
        return *this;
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    /**
     * @brief Get access to the underlying reference
     */
    [[nodiscard]] constexpr T &operator*() noexcept { return _ref; }

    /**
     * @brief Access methods of the underlying object
     */
    [[nodiscard]] constexpr T *operator->() noexcept { return &_ref; }

private:
    T &_ref; /// Reference to the tracked object

    /**
     * @brief Reference counter
     *
     * It is acquired on construction and released on destruction.
     * When @p true, no other references to the object can be borrowed.
     */
    internal::ReferenceLock &_lock;
};
} // namespace safe

#endif // SAFE_REFERENCE_MUTABLE_HPP
