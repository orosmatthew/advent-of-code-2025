#include <utils.hpp>

#include <print>

struct Range {
    uint64_t start;
    uint64_t end;
};

static Range parse_range(const std::string& data, int& pos)
{
    const auto start = parse_uint<uint64_t>(data, pos);
    ++pos; // "-"
    const auto end = parse_uint<uint64_t>(data, pos);
    return Range { start, end };
}

static uint64_t repeat_digits(const uint64_t value)
{
    return value * ten_power(count_digits(value)) + value;
}

static uint64_t invalid_id_sum(const Range range)
{
    uint64_t sum = 0;
    const int start_digits = count_digits(range.start);
    const int end_digits = count_digits(range.end);
    const Range check_range { range.start / ten_power((start_digits + (start_digits % 2 != 0 ? 1 : 0)) / 2),
                              range.end / ten_power((end_digits - (end_digits % 2 != 0 ? 1 : 0)) / 2) };
    for (uint64_t check = check_range.start; check <= check_range.end; ++check) {
        if (const uint64_t repeated = repeat_digits(check); repeated >= range.start && repeated <= range.end) {
            sum += repeated;
        }
    }
    return sum;
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
    const std::string data = read_file("./day02-part1/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 10000);
#else
    std::println("{}", solve(data));
#endif
}