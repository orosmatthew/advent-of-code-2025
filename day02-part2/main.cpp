#include <utils.hpp>

#include <algorithm>
#include <print>
#include <unordered_set>

struct Range {
    uint64_t start;
    uint64_t end;
};

static Range parse_range(const std::string& data, int& pos)
{
    const uint64_t start = parse_uint<uint64_t>(data, pos);
    ++pos; // "-"
    const uint64_t end = parse_uint<uint64_t>(data, pos);
    return Range { start, end };
}

static uint64_t repeat_digits(const uint64_t value, const uint64_t times)
{
    const int digits = count_digits(value);
    uint64_t result = 0;
    for (int i = 0; i < times; ++i) {
        result += value * ten_power(digits * i);
    }
    return result;
}

static uint64_t invalid_id_sum(const Range range)
{
    std::unordered_set<uint64_t> invalid_ids;
    const int start_digits = count_digits(range.start);
    const int end_digits = count_digits(range.end);
    const uint64_t check_end = ten_power((end_digits + 1) / 2);
    for (int check = 0; check < check_end; ++check) {
        const int check_digits = count_digits(check);
        auto check_times = [&](const int times) {
            if (times == 0 || times == 1) {
                return;
            }
            if (const uint64_t value = repeat_digits(check, times); value >= range.start && value <= range.end) {
                invalid_ids.insert(value);
            }
        };
        if (start_digits == end_digits && start_digits % check_digits == 0) {
            check_times(start_digits / check_digits);
        } else {
            if (start_digits % check_digits == 0) {
                check_times(start_digits / check_digits);
            }
            if (end_digits % check_digits == 0) {
                check_times(end_digits / check_digits);
            }
        }
    }
    return std::ranges::fold_left(invalid_ids, 0ULL, [](const uint64_t sum, const uint64_t value) {
        return sum + value;
    });
}

static uint64_t solve(const std::string& data)
{
    uint64_t sum = 0;
    for (int pos = 0; pos < data.size(); ++pos) {
        const Range range = parse_range(data, pos);
        sum += invalid_id_sum(range);
    }
    return sum;
}

int main()
{
    const std::string data = read_file("./day02-part2/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 1000);
#else
    std::println("{}", solve(data));
#endif
}