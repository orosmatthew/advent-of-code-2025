#include <utils.hpp>

#include <algorithm>
#include <print>
#include <ranges>

struct InclusiveRange {
    uint64_t start;
    uint64_t end;

    bool operator==(const InclusiveRange& other) const noexcept
    {
        return start == other.start && end == other.end;
    }

    bool operator<(const InclusiveRange& other) const noexcept
    {
        if (start != other.start) {
            return start < other.start;
        }
        return end < other.end;
    }
};

enum class RangePointType { start, end };

struct RangePoint {
    RangePointType type;
    uint64_t value;

    bool operator<(const RangePoint& other) const noexcept
    {
        if (value != other.value) {
            return value < other.value;
        }
        return type == RangePointType::start && other.type == RangePointType::end;
    }
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

static uint64_t solve(const std::string& data)
{
    int pos = 0;
    const std::vector<InclusiveRange> ranges = parse_ranges(data, pos);
    std::vector<RangePoint> points;
    points.reserve(ranges.size() * 2);
    for (const auto& [start, end] : ranges) {
        points.emplace_back<RangePoint>({ .type = RangePointType::start, .value = start });
        points.emplace_back<RangePoint>({ .type = RangePointType::end, .value = end });
    }
    std::ranges::sort(points, std::less {});
    uint64_t count = 0;
    int balance = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        if (balance == 0 && (i == 0 || points[i - 1].value != points[i].value)) {
            ++count;
        }
        const auto [type, value] = points[i];
        if (type == RangePointType::start) {
            ++balance;
        } else {
            --balance;
            assert(balance >= 0);
            if (balance <= 0) {
                continue;
            }
        }
        if (i >= points.size() - 1) {
            continue;
        }
        const auto [next_type, next_value] = points[i + 1];
        count += next_value - value;
    }
    return count;
}

int main()
{
    const std::string data = read_file("./day05-part2/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 100000);
#else
    std::println("{}", solve(data));
#endif
}