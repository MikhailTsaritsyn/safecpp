//
// Created by Mikhail Tsaritsyn on Jan 14, 2025.
//
#include "BorrowChecker.hpp"

#include <gtest/gtest.h>
#include <random>
#include <thread>

/// Test synchronization using throwing borrow API
TEST(BorrowChecker, ThrowingSync) {
    std::mt19937 gen{ 0 };
    std::uniform_int_distribution<size_t> distr(70, 120);

    safe::BorrowChecker<size_t> bc(0);

    safe::BorrowChecker<std::vector<size_t>> result{ std::vector<size_t>() };
    const auto executable = [&bc, &d1 = distr, &gen, &result](const size_t i) {
        std::uniform_int_distribution<size_t> d2(0, 100);
        std::this_thread::sleep_for(std::chrono::milliseconds(d2(gen)));
        while (true) {
            try {
                auto &x = *bc.mut();

                while (true) {
                    try {
                        auto vec = result.mut();
                        vec->push_back(x++);
                        break;
                    } catch (std::runtime_error &) { std::this_thread::sleep_for(std::chrono::milliseconds(d1(gen))); }
                }

                std::cout << std::format("Thread {}: counter by mutable = {}\n", i, x) << std::flush;
                break;
            } catch (const std::exception &) { std::this_thread::sleep_for(std::chrono::milliseconds(d1(gen))); }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(d2(gen)));
        while (true) {
            try {
                std::cout << std::format("Thread {}: counter by immutable = {}\n", i, *bc.immut()) << std::flush;
                break;
            } catch (std::runtime_error &) { std::this_thread::sleep_for(std::chrono::milliseconds(d1(gen))); }
        }
    };

    {
        std::vector<std::jthread> threads;
        threads.reserve(10);

        for (size_t i = 0; i < 10; i++) threads.emplace_back(executable, i);
    }

    EXPECT_EQ(*result.immut(), (std::vector<size_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }));
}

/// Test synchronization using non-throwing (optional) borrow API
TEST(BorrowChecker, NonThrowingSync) {
    std::mt19937 gen{ 0 };
    std::uniform_int_distribution<size_t> distr(70, 120);

    safe::BorrowChecker<size_t> bc(0);

    safe::BorrowChecker<std::vector<size_t>> result{ std::vector<size_t>() };
    const auto executable = [&bc, &d1 = distr, &gen, &result](const size_t i) {
        std::uniform_int_distribution<size_t> d2(0, 100);
        std::this_thread::sleep_for(std::chrono::milliseconds(d2(gen)));
        while (true) {
            if (auto x = bc.mut_optional()) {
                while (true) {
                    if (auto vec = result.mut_optional()) {
                        (*vec)->push_back((**x)++);
                        break;
                    } else {
                        std::this_thread::sleep_for(std::chrono::milliseconds(d1(gen)));
                    }
                }

                std::cout << std::format("Thread {}: counter by mutable = {}\n", i, **x) << std::flush;
                break;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(d1(gen)));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(d2(gen)));
        while (true) {
            if (auto x = bc.immut_optional()) {
                std::cout << std::format("Thread {}: counter by immutable = {}\n", i, *bc.immut()) << std::flush;
                break;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(d1(gen)));
            }
        }
    };

    {
        std::vector<std::jthread> threads;
        threads.reserve(10);

        for (size_t i = 0; i < 10; i++) threads.emplace_back(executable, i);
    }

    EXPECT_EQ(*result.immut(), (std::vector<size_t>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }));
}