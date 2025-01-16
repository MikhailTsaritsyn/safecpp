//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_BORROW_CHECKER_HPP
#define SAFE_BORROW_CHECKER_HPP
#include <format>
#include <iostream>
#include <optional>
#include <thread>

#include "ReferenceImmutable.hpp"
#include "ReferenceMutable.hpp"

// TODO: Make optional API the primary one and throwing API dependent on it

namespace safe {
/**
 * @brief Class that wraps a given value and tracks references to it
 *
 * At any point of time can provide:
 * - EITHER one read-write (mutable) reference
 * - OR any number of read-only (immutable) references
 *
 * But not both at once.
 *
 * @tparam T Referenced type
 */
template <typename T>
    requires(!std::is_reference_v<T>)
class BorrowChecker {
public:
    BorrowChecker() noexcept = delete;

    BorrowChecker(const BorrowChecker &other) noexcept : _value(other._value) {}

    BorrowChecker(BorrowChecker &&other) noexcept = delete;
    BorrowChecker &operator=(const BorrowChecker &other) noexcept = delete;
    BorrowChecker &operator=(BorrowChecker &&other) noexcept = delete;

    /**
     * @brief Construct a value in-place and manage references to it
     *
     * @param args Constructor arguments
     */
    template <typename... Args> constexpr explicit BorrowChecker(Args &&...args) : _value(args...) {}

    ~BorrowChecker() noexcept {
        if (_immutable_count.value() != 0 || _mutable_lock.locked()) {
            std::cerr << "Dangling reference detected\n";
            exit(160);
        }
    }

    /**
     * @brief Borrow a mutable reference to the managed value
     *
     * @throws std::runtime_error if another mutable reference has been already borrowed
     * @throws std::runtime_error if an immutable reference has been already borrowed
     */
    [[nodiscard]] constexpr ReferenceMutable<T> mut();

    /**
     * @brief Borrow a mutable reference to the managed value
     *
     * Unlike @link mut @endlink doesn't throw if it's impossible to borrow.
     * Instead, returns @p nullopt on failure.
     *
     * @return @p nullopt if and only if one of the following prevents the borrow:
     *         - Any number of immutable references has been already borrowed
     *         - Another mutable reference has been already borrowed
     */
    [[nodiscard]] constexpr std::optional<ReferenceMutable<T>> mut_optional() noexcept;

    /**
     * @brief Borrow a mutable reference to the managed value
     *
     * Unlike @link mut @endlink, retries access after a certain period of time until succeeds or the timeout exceeds.
     * Designed for synchronization across multiple threads.
     *
     * @param retry Time period between consecutive access tries
     * @param timeout Timeout after which it exits forcefully.
     *                If @p nullopt given, it tries indefinitely.
     *
     * @throws std::runtime_error if and only if timeout is given and has exceeded
     */
    [[nodiscard]] constexpr ReferenceMutable<T>
    mut_waiting(const std::chrono::system_clock::duration &retry,
                const std::optional<std::chrono::system_clock::duration> &timeout = std::nullopt) {
        return access_waiting(&BorrowChecker::mut_optional, retry, timeout);
    }

    /**
     * @brief Borrow an immutable reference to the managed value
     *
     * @throws std::runtime_error if a mutable reference has been already borrowed
     */
    [[nodiscard]] constexpr ReferenceImmutable<T> immut();

    /**
     * @brief Borrow an immutable reference to the managed value
     *
     * Unlike @link immut @endlink doesn't throw.
     * Instead, returns @p nullopt on failure.
     *
     * @return @p nullopt if and only if a mutable reference has been already borrowed
     */
    [[nodiscard]] constexpr std::optional<ReferenceImmutable<T>> immut_optional() noexcept;

    /**
     * @brief Borrow an immutable reference to the managed value
     *
     * Unlike @link immut @endlink, retries access after a certain period of time until succeeds or the timeout exceeds.
     * Designed for synchronization across multiple threads.
     *
     * @param retry Time period between consecutive access tries
     * @param timeout Timeout after which it exits forcefully.
     *                If @p nullopt given, it tries indefinitely.
     *
     * @throws std::runtime_error if and only if timeout is given and has exceeded
     */
    [[nodiscard]] constexpr ReferenceImmutable<T>
    immut_waiting(const std::chrono::system_clock::duration &retry,
                  const std::optional<std::chrono::system_clock::duration> &timeout = std::nullopt) {
        return access_waiting(&BorrowChecker::immut_optional, retry, timeout);
    }

private:
    [[nodiscard]] constexpr auto
    access_waiting(auto &&access,
                   const std::chrono::system_clock::duration &retry,
                   const std::optional<std::chrono::system_clock::duration> &timeout = std::nullopt);

    friend std::ostream &operator<<(std::ostream &os, const BorrowChecker &bc) noexcept {
        return os << std::format("BorrowChecker(mutable = {}, immutable = {})",
                                 bc._mutable_lock.locked() ? "yes" : "no",
                                 bc._immutable_count.value());
    }

    T _value;

    /**
     * @note Can't be modified inside this class, but only by borrowed @link ReferenceMutable @endlink
     */
    internal::ReferenceLock _mutable_lock{};

    /**
     * @note Can't be modified inside this class, but only by borrowed @link ReferenceImmutable @endlink
     */
    internal::ReferenceCounter _immutable_count{};
};

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr ReferenceMutable<T> BorrowChecker<T>::mut() {
    if (_immutable_count.value() != 0)
        throw std::runtime_error("Attempt to borrow a mutable reference when already borrowed an immutable one");
    return ReferenceMutable<T>(_value, _mutable_lock);
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr std::optional<ReferenceMutable<T>> BorrowChecker<T>::mut_optional() noexcept {
    // BUG: Non-atomic
    if (_immutable_count.value() != 0) return std::nullopt;
    try {
        return ReferenceMutable<T>(_value, _mutable_lock);
    } catch (std::runtime_error &) { return std::nullopt; }
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr ReferenceImmutable<T> BorrowChecker<T>::immut() {
    if (_mutable_lock.locked())
        throw std::runtime_error("Attempt to borrow an immutable reference when already borrowed a mutable one");
    return ReferenceImmutable<T>(_value, _immutable_count);
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr std::optional<ReferenceImmutable<T>> BorrowChecker<T>::immut_optional() noexcept {
    // BUG: Non-atomic
    if (_mutable_lock.locked()) return std::nullopt;
    return ReferenceImmutable<T>(_value, _immutable_count);
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto BorrowChecker<T>::access_waiting(auto &&access,
                                                const std::chrono::system_clock::duration &retry,
                                                const std::optional<std::chrono::system_clock::duration> &timeout) {
    const auto startTime       = std::chrono::system_clock::now();
    const auto should_continue = [&timeout, &startTime] noexcept {
        if (timeout) return std::chrono::system_clock::now() < startTime + *timeout;
        return true;
    };

    while (should_continue()) {
        if (auto result = (this->*access)()) return *std::move(result);
        std::this_thread::sleep_for(retry);
    }

    throw std::runtime_error("Timeout exceeded");
}
} // namespace safe

#endif // SAFE_BORROW_CHECKER_HPP
