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
              _count(count) {
        _count.inc();
    }

    /**
     * @brief Copy the wrapper and increase the reference counter
     */
    constexpr ReferenceImmutable(const ReferenceImmutable &other) noexcept : _ref(other._ref), _count(other._count) {
        _count.inc();
    }

    ReferenceImmutable &operator=(const ReferenceImmutable &) noexcept = delete;
    ReferenceImmutable(ReferenceImmutable &&other) noexcept            = delete;
    ReferenceImmutable &operator=(ReferenceImmutable &&) noexcept      = delete;

    /**
     * @brief Decrease the reference counter
     */
    constexpr ~ReferenceImmutable() noexcept {
        try {
            _count.dec();
        } catch (std::runtime_error &) { assert("Reference count is already zero" && false); }
    }

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
     * @note Is increased exactly once in the constructor and decreased exactly
     * once in the destructor
     */
    internal::ReferenceCounter<size_t> &_count;
};

} // namespace safe

#endif // SAFE_REFERENCE_IMMUTABLE_HPP
