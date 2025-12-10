#include <utils.hpp>

#include <print>

struct Vector2i64 {
    int64_t x;
    int64_t y;
};

static std::vector<Vector2i64> parse_positions(const std::string& data)
{
    std::vector<Vector2i64> positions;
    for (int pos = 0; pos < data.size(); ++pos) {
        const int64_t x = parse_uint<uint64_t>(data, pos);
        assert(data[pos] == ',');
        ++pos;
        const int64_t y = parse_uint<uint64_t>(data, pos);
        assert(data[pos] == '\n');
        positions.emplace_back(x, y);
    }
    return positions;
}

static uint64_t rect_area(const Vector2i64 first, const Vector2i64 second)
{
    const uint64_t width = std::abs(second.x - first.x) + 1;
    const uint64_t height = std::abs(second.y - first.y) + 1;
    return width * height;
}

static uint64_t solve(const std::string& data)
{
    const std::vector<Vector2i64> positions = parse_positions(data);
    uint64_t max_area = std::numeric_limits<uint64_t>::lowest();
    for (int i = 0; i < positions.size(); ++i) {
        for (int j = i + 1; j < positions.size(); ++j) {
            max_area = std::max(max_area, rect_area(positions[i], positions[j]));
        }
    }
    return max_area;
}

int main()
{
    const std::string data = read_file("./day09-part1/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 10000);
#else
    std::println("{}", solve(data));
#endif
}