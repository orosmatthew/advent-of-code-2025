#include <utils.hpp>

#include <print>

struct Rotation {
    int sign;
    int value;
};

static Rotation parse_rotation(const std::string& data, int& pos)
{
    const int sign = data[pos] == 'L' ? -1 : 1;
    ++pos;
    const int value = parse_uint<int>(data, pos);
    return Rotation { sign, value };
}

static int solve(const std::string& data)
{
    int zero_count = 0;
    int dial = 50;
    for (int pos = 0; pos < data.length(); ++pos) {
        const auto [sign, rotation] = parse_rotation(data, pos);
        const int to_zero_amount = dial != 0 ? sign < 0 ? dial : 100 - dial : 100;
        const int init_amount = std::min(to_zero_amount, rotation);
        dial = math_mod(dial + sign * init_amount, 100);
        if (dial == 0) {
            ++zero_count;
        }
        const int remainder = rotation - init_amount;
        dial = math_mod(dial + sign * remainder, 100);
        zero_count += remainder / 100;
    }
    return zero_count;
}

int main()
{
    const std::string data = read_file("./day01-part2/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 10000);
#else
    std::println("{}", solve(data));
#endif
}