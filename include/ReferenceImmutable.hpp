//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_REFERENCE_IMMUTABLE_HPP
#define SAFE_REFERENCE_IMMUTABLE_HPP
#include "internal/ReferenceTracker.hpp"
#include <concepts>
#include <iostream>

namespace safe {
/**
 * @brief Wrapper around read-only reference to a value
 *
 * @tparam T Referenced type
 */
template <typename T>
    requires(!std::is_reference_v<T>)
class ReferenceImmutable {
public:
    ReferenceImmutable() = delete;

    ReferenceImmutable(const ReferenceImmutable &other) noexcept : _ref(other._ref), _tracker(other._tracker) {
        if (_tracker && !_tracker->register_immutable()) {
            // Since an existing immutable reference is copied, it means that there can be no mutable references.
            // Therefore, nothing can prevent registering another immutable reference.
            // If it happens, it can only mean a bug in the implementation of this library, not in the used code.
            std::cerr << "Failed to register a copy of an immutable reference\n";
            exit(162);
        }
    }

    ReferenceImmutable &operator=(const ReferenceImmutable &) noexcept = delete;

    ReferenceImmutable(ReferenceImmutable &&other) noexcept : _ref(other._ref), _tracker(other._tracker) {
        other._tracker = nullptr;
    }

    ReferenceImmutable &operator=(ReferenceImmutable &&other) noexcept = delete;

    ~ReferenceImmutable() noexcept {
        if (_tracker && !_tracker->unregister_immutable()) {
            std::cerr << "Double release of an immutable reference" << std::endl;
            exit(161);
        }
    }

    ReferenceImmutable(T &ref, internal::ReferenceTracker &tracker) noexcept : _ref(ref), _tracker(&tracker) {}

    /**
     * @brief Get access to the underlying reference
     */
    [[nodiscard]] constexpr const T &operator*() const noexcept { return _ref; }

    /**
     * @brief Access methods of the underlying object
     */
    [[nodiscard]] constexpr const T *operator->() const noexcept { return &_ref; }

private:
    const T &_ref; /// Reference to the tracked object
    internal::ReferenceTracker *_tracker;
};
} // namespace safe

#endif // SAFE_REFERENCE_IMMUTABLE_HPP
