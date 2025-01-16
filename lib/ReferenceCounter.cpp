//
// Created by Mikhail Tsaritsyn on Jan 16, 2025.
//
#include "internal/ReferenceCounter.hpp"

#include <stdexcept>

namespace safe::internal {
void ReferenceCounter::inc() noexcept {
    std::lock_guard guard(_mutex);
    ++_value;
}

void ReferenceCounter::dec() {
    std::lock_guard guard(_mutex);
    if (_value == 0) throw std::runtime_error("Counter value is zero");
    --_value;
}
} // namespace safe::internal
