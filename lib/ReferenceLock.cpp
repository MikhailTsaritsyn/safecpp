//
// Created by Mikhail Tsaritsyn on Jan 16, 2025.
//
#include "internal/ReferenceLock.hpp"

#include <stdexcept>

namespace safe::internal {
void ReferenceLock::lock() {
    _mutex.lock();
    if (_locked) {
        _mutex.unlock();
        throw std::runtime_error("Lock is already acquired");
    }
    _locked = true;
    _mutex.unlock();
}

void ReferenceLock::unlock() {
    _mutex.lock();
    if (!_locked) {
        _mutex.unlock();
        throw std::runtime_error("Lock is already released");
    }
    _locked = false;
    _mutex.unlock();
}
} // namespace safe::internal
