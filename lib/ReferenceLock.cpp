//
// Created by Mikhail Tsaritsyn on Jan 16, 2025.
//
#include "internal/ReferenceLock.hpp"

namespace safe::internal {
bool ReferenceLock::lock() noexcept {
    std::lock_guard guard(_mutex);
    if (_locked) return false;
    _locked = true;
    return true;
}

bool ReferenceLock::unlock() noexcept {
    std::lock_guard guard(_mutex);
    if (!_locked) return false;
    _locked = false;
    return true;
}
} // namespace safe::internal
