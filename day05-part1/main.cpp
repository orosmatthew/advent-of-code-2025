#include <utils.hpp>

#include <algorithm>
#include <print>

struct InclusiveRange {
    uint64_t start;
    uint64_t end;
};

static std::vector<InclusiveRange> parse_ranges(const std::string& data, int& pos)
{
    std::vector<InclusiveRange> ranges;
    for (; pos < data.size(); ++pos) {
        if (data[pos] == '\n') {
            break;
        }
        const auto start = parse_uint<uint64_t>(data, pos);
        ++pos; // "-"
        const auto end = parse_uint<uint64_t>(data, pos);
        ranges.emplace_back<InclusiveRange>({ start, end });
    }
    return ranges;
}

static bool id_valid(const std::vector<InclusiveRange>& ranges, const uint64_t id)
{
    return std::ranges::any_of(ranges, [id](const InclusiveRange& range) {
        return id >= range.start && id <= range.end;
    });
}

static int solve(const std::string& data)
{
    int pos = 0;
    const std::vector<InclusiveRange> ranges = parse_ranges(data, pos);
    ++pos; // "\n"
    int valid_count = 0;
    for (; pos < data.size(); ++pos) {
        if (const auto id = parse_uint<uint64_t>(data, pos); id_valid(ranges, id)) {
            ++valid_count;
        }
    }
    return valid_count;
}

int main()
{
    const std::string data = read_file("./day05-part1/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 10000);
#else
    std::println("{}", solve(data));
#endif
}