//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_REFERENCE_MUTABLE_HPP
#define SAFE_REFERENCE_MUTABLE_HPP
#include "internal/ARC.hpp"
#include <concepts>
#include <iostream>

namespace safe {
/**
 * @brief Wrapper around read-write reference to a value
 *
 * @tparam T Referenced type
 */
template <typename T>
    requires(!std::is_reference_v<T>)
class MutRef {
public:
    MutRef() = delete;

    MutRef(const MutRef &) noexcept            = delete;
    MutRef &operator=(const MutRef &) noexcept = delete;

    MutRef(MutRef &&other) noexcept : _ref(other._ref), _tracker(other._tracker) { other._tracker = nullptr; }

    MutRef &operator=(MutRef &&other) noexcept {
        _ref = other._ref;
        std::swap(_tracker, other._tracker);
        return *this;
    }

    ~MutRef() noexcept {
        if (_tracker && !_tracker->unregister_mutable()) {
            std::cerr << "Double release of a mutable reference" << std::endl;
            exit(161);
        }
    }

    MutRef(T &ref, internal::ARC &tracker) noexcept : _ref(ref), _tracker(&tracker) {}

    /**
     * @brief Get access to the underlying reference
     */
    [[nodiscard]] constexpr T &operator*() noexcept { return _ref; }

    /**
     * @brief Access methods of the underlying object
     */
    [[nodiscard]] constexpr T *operator->() noexcept { return &_ref; }

private:
    T &_ref;                 ///< Reference to the tracked object
    internal::ARC *_tracker; ///< Counter shared among all references to the object
};
} // namespace safe

#endif // SAFE_REFERENCE_MUTABLE_HPP
