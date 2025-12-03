#include <utils.hpp>

#include <algorithm>
#include <print>
#include <span>

static void parse_battery_bank(const std::string& data, int& pos, std::vector<uint8_t>& batteries)
{
    batteries.clear();
    while (is_digit(data[pos])) {
        batteries.push_back(data[pos] - '0');
        ++pos;
    }
}

static uint64_t combine_digits(const uint64_t first, const uint64_t second)
{
    return first * ten_power(count_digits(second)) + second;
}

static uint64_t calc_largest_joltage(std::span<const uint8_t> batteries, const int count)
{
    assert(count > 0);
    const std::span search_span { batteries.data(), batteries.size() - count + 1 };
    const auto max_it = std::ranges::max_element(search_span);
    uint64_t max_joltage = *max_it;
    if (count > 1) {
        assert(batteries.size() > 1);
        const std::span<uint8_t>::difference_type max_offset = std::distance(search_span.begin(), max_it);
        const std::span next_span { batteries.begin() + max_offset + 1, batteries.end() };
        const uint64_t next_max_joltage = calc_largest_joltage(next_span, count - 1);
        max_joltage = combine_digits(max_joltage, next_max_joltage);
    }
    return max_joltage;
}

static uint64_t solve(const std::string& data)
{
    uint64_t sum = 0;
    static std::vector<uint8_t> batteries;
    for (int pos = 0; pos < data.size(); ++pos) {
        parse_battery_bank(data, pos, batteries);
        sum += calc_largest_joltage(batteries, 2);
    }
    return sum;
}

int main()
{
    const std::string data = read_file("./day03-part2/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 10000);
#else
    std::println("{}", solve(data));
#endif
}