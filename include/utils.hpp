#pragma once

#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <print>
#include <string>

inline std::string read_file(const std::filesystem::path& path)
{
    const std::ifstream file { path };
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

template <typename Func>
void benchmark(Func func, const int runs)
{
    double time_running_total = 0.0;
    for (int i = 0; i < runs; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        volatile auto result = std::invoke(func);
        auto end = std::chrono::high_resolution_clock::now();
        time_running_total += std::chrono::duration<double, std::nano>(end - start).count();
    }
    int avg_ns = static_cast<int>(std::round(time_running_total / runs));
    std::println("Iterations: {}, Average ns: {}", runs, avg_ns);
}

inline bool is_digit(const char c)
{
    return c >= '0' && c <= '9';
}

template <typename UInt>
UInt parse_uint(const std::string& data, int& pos)
{
    UInt result = 0;
    while (is_digit(data[pos])) {
        result = result * 10 + (data[pos] - '0');
        ++pos;
    }
    return result;
}

template <typename Int>
constexpr Int math_mod(const Int dividend, const Int divisor)
{
    const Int remainder = dividend % divisor;
    if (remainder < 0) {
        return remainder + std::abs(divisor);
    }
    return remainder;
}

inline int count_digits(uint64_t value)
{
    int n = 1;
    if (value >= 10000000000000000) {
        n += 16;
        value /= 10000000000000000;
    }
    if (value >= 100000000) {
        n += 8;
        value /= 100000000;
    }
    if (value >= 10000) {
        n += 4;
        value /= 10000;
    }
    if (value >= 100) {
        n += 2;
        value /= 100;
    }
    if (value >= 10) {
        ++n;
    }
    return n;
}

static uint64_t ten_power(const int exponent)
{
    switch (exponent) {
    case 0:
        return 1;
    case 1:
        return 10;
    case 2:
        return 100;
    case 3:
        return 1000;
    case 4:
        return 10000;
    case 5:
        return 100000;
    case 6:
        return 1000000;
    case 7:
        return 10000000;
    case 8:
        return 100000000;
    case 9:
        return 1000000000;
    case 10:
        return 10000000000;
    case 11:
        return 100000000000;
    case 12:
        return 1000000000000;
    case 13:
        return 10000000000000;
    case 14:
        return 100000000000000;
    case 15:
        return 1000000000000000;
    case 16:
        return 10000000000000000;
    case 17:
        return 100000000000000000;
    case 18:
        return 1000000000000000000;
    case 19:
        return 10000000000000000000ULL;
    default:
        assert(false);
        return 0;
    }
}

inline size_t hash_combine(const size_t first, const size_t second)
{
    // Based on Boost's hash_combine
    return first ^ second + 0x9e3779b9 + (first << 6) + (first >> 2);
}