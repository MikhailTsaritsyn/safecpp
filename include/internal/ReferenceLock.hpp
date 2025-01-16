//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_REFERENCE_LOCK_HPP
#define SAFE_REFERENCE_LOCK_HPP
#include <mutex>

namespace safe::internal {
/**
 * @brief Thread-safe lock for unique references
 *
 * Has only two states: locked and unlocked.
 * When it is locked, it can't be locked again until it is unlocked.
 * When it is unlocked, it can't be unlocked again until it is locked.
 */
class ReferenceLock {
public:
    /**
     * Sets the lock to open
     */
    constexpr ReferenceLock() noexcept                                 = default;
    constexpr ReferenceLock(const ReferenceLock &) noexcept            = delete;
    constexpr ReferenceLock(ReferenceLock &&) noexcept                 = delete;
    constexpr ReferenceLock &operator=(const ReferenceLock &) noexcept = delete;
    constexpr ReferenceLock &operator=(ReferenceLock &&) noexcept      = delete;

    /**
     * @return @p true if and only if executed successfully
     */
    [[nodiscard]] bool lock() noexcept;

    /**
     * @return @p true if and only if executed successfully
     */
    [[nodiscard]] bool unlock() noexcept;

    [[nodiscard]] constexpr bool locked() const noexcept { return _locked; }

private:
    bool _locked = false;
    std::mutex _mutex{}; ///< Mutex guarding lock state changes
};
} // namespace safe::internal

#endif // SAFE_REFERENCE_LOCK_HPP
