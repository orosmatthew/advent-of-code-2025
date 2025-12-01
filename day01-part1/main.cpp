#include "../utils.hpp"

#include <print>

static int solve(const std::string& data)
{
    return 1;
}

int main()
{
    const std::string data = read_file("./day01-part1/sample.txt");
#ifdef BENCHMARK
    benchmark([&] { solve(data); }, 1000);
#else
    std::println("{}", solve(data));
#endif
}