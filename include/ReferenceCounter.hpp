//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_REFERENCE_COUNTER_HPP
#define SAFE_REFERENCE_COUNTER_HPP
#include <concepts>
#include <cstddef>
#include <mutex>
#include <stdexcept>

namespace safe::internal {

/**
 * @brief Thread-safe reference counter
 *
 * @tparam T Unsigned integral type of the value, @p size_t by default
 */
template <typename T = size_t>
    requires(std::is_unsigned_v<T> && std::is_integral_v<T>)
class ReferenceCounter {
public:
    /**
     * Sets the counter to zero
     */
    constexpr ReferenceCounter() = default;

    /**
     * @brief Increment the counter
     */
    constexpr void inc() noexcept {
        std::lock_guard guard(_mutex);
        ++_value;
    }

    /**
     * @brief Decrement the counter
     *
     * @throws std::runtime_error if the counter is already zero
     */
    constexpr void dec() {
        std::lock_guard guard(_mutex);
        if (_value == 0) throw std::invalid_argument("Counter value is zero");
        --_value;
    }

    /**
     * @return The current value of the counter
     */
    [[nodiscard]] constexpr T value() const noexcept { return _value; }

private:
    T _value = 0;
    std::mutex _mutex{}; ///< Mutex guarding increment and decrement of the counter
};

} // namespace safe::internal

#endif // SAFE_REFERENCE_COUNTER_HPP
