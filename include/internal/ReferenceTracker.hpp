//
// Created by Mikhail Tsaritsyn on Jan 16, 2025.
//

#ifndef SAFE_REFERENCE_GUARD_HPP
#define SAFE_REFERENCE_GUARD_HPP
#include <mutex>

namespace safe::internal {
/**
 * @brief Class keeping track of all reference borrow events
 *
 * The rules for registering/unregistering references are listed in @link BorrowChecker @endlink
 */
class ReferenceTracker {
public:
    enum struct MutableRegisterStatus { SUCCESS, MUTABLE_EXISTS, IMMUTABLE_EXISTS };

    ReferenceTracker() noexcept = default;

    ReferenceTracker(const ReferenceTracker &) noexcept            = delete;
    ReferenceTracker &operator=(const ReferenceTracker &) noexcept = delete;
    ReferenceTracker(ReferenceTracker &&) noexcept                 = delete;
    ReferenceTracker &operator=(ReferenceTracker &&) noexcept      = delete;

    /**
     * @note Terminates execution with code 161 if there are registered references remaining
     */
    ~ReferenceTracker() noexcept;

    /**
     * @brief Register that a mutable reference has been borrowed
     *
     * In a case of failure does nothing.
     *
     * @return One of the following status values:
     *         - @p MUTABLE_EXISTS if another mutable reference has already been borrowed
     *         - @p IMMUTABLE_EXISTS if one or more immutable references have already been borrowed
     *         - @p SUCCESS otherwise
     */
    [[nodiscard]] MutableRegisterStatus register_mutable() noexcept;

    /**
     * @brief Remove the record of the mutable reference
     *
     * In a case of failure does nothing.
     *
     * @return @p false if and only if there's no registered mutable reference
     */
    [[nodiscard]] bool unregister_mutable() noexcept;

    /**
     * @brief Add a record of an immutable reference
     *
     * In a case of failure does nothing.
     *
     * @return @p false if and only if a mutable reference has already been registered
     */
    [[nodiscard]] bool register_immutable() noexcept;

    /**
     * @brief Remove a record of an immutable reference
     *
     * In a case of failure does nothing.
     *
     * @return @p false if and only if there are no immutable references registered
     */
    [[nodiscard]] bool unregister_immutable() noexcept;

    /**
     * @return @p true if and only if there's a mutable reference registered
     */
    [[nodiscard]] bool mutable_registered() const noexcept;

    /**
     * @return The number of registered mutable references
     */
    [[nodiscard]] size_t immutables_counter() const noexcept;

private:
    bool _mutable_registered   = false; ///< Record of registered mutable reference, at most one at a time
    size_t _immutables_counter = 0;     ///< Record of registered immutable reference, any number at a time
    std::mutex _mutex{};                ///< Mutex protecting all register/unregister operations
};

} // namespace safe::internal

#endif // SAFE_REFERENCE_GUARD_HPP
