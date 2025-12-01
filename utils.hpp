#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
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
        std::invoke(func);
        auto end = std::chrono::high_resolution_clock::now();
        time_running_total += std::chrono::duration<double, std::nano>(end - start).count();
    }
    int avg_ns = static_cast<int>(std::round(time_running_total / runs));
    std::println("Average ns: {}", avg_ns);
}