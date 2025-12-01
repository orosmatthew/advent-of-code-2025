#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <functional>
#include <cmath>

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

inline int parse_int(const std::string& data, size_t& pos)
{
    int result = 0;
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