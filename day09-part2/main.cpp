#include <utils.hpp>

#include <generator>
#include <print>

// ReSharper disable CppUseStructuredBinding

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

static std::vector<Vector2i64> get_perimeter_positions(const std::vector<Vector2i64>& positions)
{
    std::vector<Vector2i64> result;

    auto add_between = [&](const Vector2i64 start, const Vector2i64 end) {
        const int64_t dx = end.x - start.x;
        const int64_t dy = end.y - start.y;
        assert((dx == 0 || dy == 0) && (dx != 0 || dy != 0));
        const int8_t inc_x = dx == 0 ? 0 : dx > 0 ? 1 : -1;
        const int8_t inc_y = dy == 0 ? 0 : dy > 0 ? 1 : -1;
        assert((inc_x == 0 || inc_y == 0) && (inc_x != 0 || inc_y != 0));
        if (inc_x != 0) {
            for (int64_t x = start.x; inc_x > 0 ? x < end.x : x > end.x; x += inc_x) {
                result.emplace_back(x, start.y);
            }
        } else if (inc_y != 0) {
            for (int64_t y = start.y; inc_y > 0 ? y < end.y : y > end.y; y += inc_y) {
                result.emplace_back(start.x, y);
            }
        } else {
            assert(false);
        }
    };

    for (size_t i = 1; i < positions.size(); ++i) {
        const Vector2i64 start = positions[i - 1];
        const Vector2i64 end = positions[i];
        add_between(start, end);
    }
    add_between(positions[positions.size() - 1], positions[0]);
    return result;
}

static uint64_t solve(const std::string& data)
{
    const std::vector<Vector2i64> positions = parse_positions(data);
    const std::vector<Vector2i64> perimeter_positions = get_perimeter_positions(positions);
    uint64_t max_area = std::numeric_limits<uint64_t>::lowest();
    for (size_t i = 0; i < perimeter_positions.size(); ++i) {
        for (size_t j = i + 1; j < perimeter_positions.size(); ++j) {
            max_area = std::max(max_area, rect_area(perimeter_positions[i], perimeter_positions[j]));
        }
    }
    return max_area;
}

int main()
{
    const std::string data = read_file("./day09-part2/sample.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 10000);
#else
    std::println("{}", solve(data));
#endif
}