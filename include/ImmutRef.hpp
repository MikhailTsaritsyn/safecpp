//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_REFERENCE_IMMUTABLE_HPP
#define SAFE_REFERENCE_IMMUTABLE_HPP
#include "internal/ARC.hpp"
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
class ImmutRef {
public:
    ImmutRef() = delete;

    ImmutRef(const ImmutRef &other) noexcept : _ref(other._ref), _arc(other._arc) {
        if (_arc && !_arc->register_immutable()) {
            // Since an existing immutable reference is copied, it means that there can be no mutable references.
            // Therefore, nothing can prevent registering another immutable reference.
            // If it happens, it can only mean a bug in the implementation of this library, not in the used code.
            std::cerr << "Failed to register a copy of an immutable reference\n";
            exit(162);
        }
    }

    ImmutRef &operator=(const ImmutRef &) noexcept = delete;

    ImmutRef(ImmutRef &&other) noexcept : _ref(other._ref), _arc(other._arc) { other._arc = nullptr; }

    ImmutRef &operator=(ImmutRef &&other) noexcept = delete;

    ~ImmutRef() noexcept {
        if (_arc && !_arc->unregister_immutable()) {
            std::cerr << "Double release of an immutable reference" << std::endl;
            exit(161);
        }
    }

    ImmutRef(T &ref, internal::ARC &tracker) noexcept : _ref(ref), _arc(&tracker) {}

    /**
     * @brief Get access to the underlying reference
     */
    [[nodiscard]] constexpr const T &operator*() const noexcept { return _ref; }

    /**
     * @brief Access methods of the underlying object
     */
    [[nodiscard]] constexpr const T *operator->() const noexcept { return &_ref; }

private:
    const T &_ref;       ///< Reference to the tracked object
    internal::ARC *_arc; ///< Counter shared among all references to the object
};
} // namespace safe

#endif // SAFE_REFERENCE_IMMUTABLE_HPP
