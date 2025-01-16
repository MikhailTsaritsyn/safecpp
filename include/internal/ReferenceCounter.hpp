//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//

#ifndef SAFE_REFERENCE_COUNTER_HPP
#define SAFE_REFERENCE_COUNTER_HPP
#include <cstddef>
#include <mutex>

namespace safe::internal {

/**
 * @brief Thread-safe reference counter
 */
class ReferenceCounter {
public:
    /**
     * Sets the counter to zero
     */
    constexpr ReferenceCounter() noexcept                                    = default;
    constexpr ReferenceCounter(const ReferenceCounter &) noexcept            = delete;
    constexpr ReferenceCounter(ReferenceCounter &&) noexcept                 = delete;
    constexpr ReferenceCounter &operator=(const ReferenceCounter &) noexcept = delete;
    constexpr ReferenceCounter &operator=(ReferenceCounter &&) noexcept      = delete;

    /**
     * @brief Increment the counter
     */
    void inc() noexcept;

    /**
     * @brief Decrement the counter
     *
     * @throws std::runtime_error if the counter is already zero
     */
    void dec();

    /**
     * @return The current value of the counter
     */
    [[nodiscard]] constexpr size_t value() const noexcept { return _value; }

private:
    size_t _value = 0;
    std::mutex _mutex{}; ///< Mutex guarding increment and decrement of the counter
};
} // namespace safe::internal

#endif // SAFE_REFERENCE_COUNTER_HPP
