//
// Created by Mikhail Tsaritsyn on Jan 16, 2025.
//
#include "internal/ReferenceCounter.hpp"

namespace safe::internal {
void ReferenceCounter::inc() noexcept {
    std::lock_guard guard(_mutex);
    ++_value;
}

bool ReferenceCounter::dec() noexcept {
    std::lock_guard guard(_mutex);
    if (_value == 0) return false;
    --_value;
    return true;
}
} // namespace safe::internal
