//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_BORROW_CHECKER_HPP
#define SAFE_BORROW_CHECKER_HPP
#include <format>
#include <optional>

#include "ReferenceImmutable.hpp"
#include "ReferenceMutable.hpp"

// TODO: Mechanism to prevent dangling references?

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
template<typename T>
    requires(!std::is_reference_v<T>)
class BorrowChecker {
public:
    BorrowChecker() noexcept = delete;
    BorrowChecker(const BorrowChecker &) noexcept = delete;
    BorrowChecker &operator=(const BorrowChecker &) noexcept = delete;
    BorrowChecker(BorrowChecker &&other) noexcept = delete;
    BorrowChecker &operator=(BorrowChecker &&other) noexcept = delete;

    /**
     * @brief Construct a value in-place and manage references to it
     *
     * @param args Constructor arguments
     */
    template<typename... Args>
    constexpr explicit BorrowChecker(Args &&...args) : _value(args...) {}

    /**
     * @brief Borrow a mutable reference to the managed value
     *
     * @throws std::runtime_error if another mutable reference has been already borrowed
     * @throws std::runtime_error if an immutable reference has been already borrowed
     */
    [[nodiscard]] constexpr ReferenceMutable<T> mut() {
        if (_immutable_count.value() != 0)
            throw std::runtime_error("Attempt to borrow a mutable reference when already borrowed an immutable one");
        return ReferenceMutable<T>(_value, _mutable_lock);
    }

    /**
     * @brief Borrow a mutable reference to the managed value
     *
     * Unlike @link mut @endlink does not throw if it is impossible to borrow.
     * Instead, returns @p nullopt on failure.
     *
     * @return @p nullopt if and only if one of the following prevents the borrow:
     *         - Any number of immutable references has been already borrowed
     *         - Another mutable reference has been already borrowed
     */
    [[nodiscard]] constexpr std::optional<ReferenceMutable<T>> mut_optional() noexcept {
        if (_immutable_count.value() != 0) return std::nullopt;
        if (_mutable_lock.locked()) return std::nullopt;
        return std::make_optional<ReferenceMutable<T>>(_value, _mutable_lock);
    }

    /**
     * @brief Borrow an immutable reference to the managed value
     *
     * @throws std::runtime_error if a mutable reference has been already borrowed
     */
    [[nodiscard]] constexpr ReferenceImmutable<T> immut() {
        if (_mutable_lock.locked())
            throw std::runtime_error("Attempt to borrow an immutable reference when already borrowed a mutable one");
        return ReferenceImmutable<T>(_value, _immutable_count);
    }

    /**
     * @brief Borrow an immutable reference to the managed value
     *
     * Unlike @link immut @endlink does not throw.
     * Instead, returns @p nullopt on failure.
     *
     * @return @p nullopt if and only if a mutable reference has been already borrowed
     */
    [[nodiscard]] constexpr std::optional<ReferenceImmutable<T>> immut_optional() noexcept {
        if (_mutable_lock.locked()) return std::nullopt;
        return std::make_optional<ReferenceImmutable<T>>(_value, _immutable_count);
    }

private:
    friend std::ostream &operator<<(std::ostream &os, const BorrowChecker &bc) noexcept {
        return os << std::format("BorrowChecker(mutable = {}, immutable = {})",
                                 bc._mutable_lock.locked() ? "yes" : "no",
                                 bc._immutable_count.value());
    }

    T _value;

    /**
     * @note Cannot be modified inside this class, but only by borrowed @link ReferenceMutable @endlink
     */
    internal::ReferenceLock _mutable_lock{};

    /**
     * @note Cannot be modified inside this class, but only by borrowed @link ReferenceImmutable @endlink
     */
    internal::ReferenceCounter<size_t> _immutable_count{};
};
} // namespace safe

#endif // SAFE_BORROW_CHECKER_HPP
