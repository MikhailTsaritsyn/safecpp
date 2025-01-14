//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_REFERENCE_LOCK_HPP
#define SAFE_REFERENCE_LOCK_HPP
#include <mutex>
#include <stdexcept>

namespace safe::internal {
/**
 * @brief Thread-safe lock for unique references
 *
 * Has only two states: locked and unlocked.
 * When it is locked, it cannot be locked again until it is unlocked.
 * When it is unlocked, it cannot be unlocked again until it is locked.
 */
class ReferenceLock {
public:
    /**
     * Sets the lock to open
     */
    constexpr ReferenceLock() = default;

    /**
     * @throws std::runtime_error If the lock is already acquired
     */
    constexpr void lock() {
        _mutex.lock();
        if (_locked) {
            _mutex.unlock();
            throw std::runtime_error("Lock is already acquired");
        }
        _locked = true;
        _mutex.unlock();
    }

    /**
     * @throws std::runtime_error If the lock is already released
     */
    constexpr void unlock() {
        _mutex.lock();
        if (!_locked) {
            _mutex.unlock();
            throw std::runtime_error("Lock is already released");
        }
        _locked = false;
        _mutex.unlock();
    }

    [[nodiscard]] constexpr bool locked() const noexcept { return _locked; }

private:
    bool _locked = false;
    std::mutex _mutex{}; ///< Mutex guarding lock state changes
};
} // namespace safe::internal

#endif // SAFE_REFERENCE_LOCK_HPP
