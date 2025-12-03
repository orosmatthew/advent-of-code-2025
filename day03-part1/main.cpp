#include <utils.hpp>

#include <print>

static void parse_battery_bank(const std::string& data, int& pos, std::vector<uint8_t>& batteries)
{
    batteries.clear();
    while (is_digit(data[pos])) {
        batteries.push_back(data[pos] - '0');
        ++pos;
    }
}

static int calc_largest_joltage(const std::vector<uint8_t>& batteries)
{
    int max_joltage = std::numeric_limits<int>::lowest();
    for (int i = 0; i < batteries.size(); ++i) {
        for (int j = i + 1; j < batteries.size(); ++j) {
            const int combined = batteries[i] * 10 + batteries[j];
            max_joltage = std::max(max_joltage, combined);
        }
    }
    return max_joltage;
}

static int solve(const std::string& data)
{
    int sum = 0;
    std::vector<uint8_t> batteries;
    for (int pos = 0; pos < data.size(); ++pos) {
        parse_battery_bank(data, pos, batteries);
        sum += calc_largest_joltage(batteries);
    }
    return sum;
}

int main()
{
    const std::string data = read_file("./day03-part1/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 10000);
#else
    std::println("{}", solve(data));
#endif
}