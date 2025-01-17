//
// Created by Mikhail Tsaritsyn on Jan 16, 2025.
//

#include "internal/ReferenceTracker.hpp"

#include <iostream>

namespace safe::internal {
ReferenceTracker::~ReferenceTracker() noexcept {
    if (_mutable_registered) {
        std::cerr << "Dangling mutable reference detected\n";
        exit(160);
    }
    if (_immutables_counter != 0) {
        std::cerr << _immutables_counter << " dangling immutable reference(s) detected\n";
        exit(160);
    }
}

ReferenceTracker::MutableRegisterStatus ReferenceTracker::register_mutable() noexcept {
    std::lock_guard guard(_mutex);
    if (_mutable_registered) return MutableRegisterStatus::MUTABLE_EXISTS;
    if (_immutables_counter != 0) return MutableRegisterStatus::IMMUTABLE_EXISTS;
    _mutable_registered = true;
    return MutableRegisterStatus::SUCCESS;
}

bool ReferenceTracker::unregister_mutable() noexcept {
    std::lock_guard guard(_mutex);
    if (!_mutable_registered) return false;
    _mutable_registered = false;
    return true;
}

bool ReferenceTracker::register_immutable() noexcept {
    std::lock_guard guard(_mutex);
    if (_mutable_registered) return false;
    _immutables_counter++;
    return true;
}

bool ReferenceTracker::unregister_immutable() noexcept {
    std::lock_guard guard(_mutex);
    if (_immutables_counter == 0) return false;
    _immutables_counter--;
    return true;
}

bool ReferenceTracker::mutable_registered() const noexcept { return _mutable_registered; }

size_t ReferenceTracker::immutables_counter() const noexcept { return _immutables_counter; }
} // namespace safe::internal