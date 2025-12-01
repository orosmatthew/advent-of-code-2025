#include "../utils.hpp"

#include <print>

static int parse_rotation(const std::string& data, size_t& pos)
{
    const int sign = data[pos] == 'L' ? -1 : 1;
    ++pos;
    const int value = parse_int(data, pos);
    return sign * value;
}

static int solve(const std::string& data)
{
    int zero_count = 0;
    int dial = 50;
    for (size_t pos = 0; pos < data.length(); ++pos) {
        const int rotation = parse_rotation(data, pos);
        dial = math_mod(dial + rotation, 100);
        if (dial == 0) {
            ++zero_count;
        }
    }
    return zero_count;
}

int main()
{
    const std::string data = read_file("./day01-part1/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 10000);
#else
    std::println("{}", solve(data));
#endif
}