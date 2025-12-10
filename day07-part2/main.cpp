#include <utils.hpp>

#include <print>
#include <unordered_map>

struct Vector2i {
    int x;
    int y;

    bool operator==(const Vector2i& other) const
    {
        return x == other.x && y == other.y;
    }

    struct Hash {
        size_t operator()(const Vector2i& vector) const noexcept
        {
            // Base on Boost hash_combine.
            size_t seed = std::hash<int>()(vector.x);
            seed ^= std::hash<int>()(vector.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
};

enum class GridState : uint8_t { empty, splitter };

struct Grid {
    Vector2i size;
    Vector2i start;
    std::vector<GridState> data;

    [[nodiscard]] const GridState& at(const Vector2i& pos) const
    {
        return data[pos.x + size.x * pos.y];
    }

    GridState& at(const Vector2i& pos)
    {
        return data[pos.x + size.x * pos.y];
    }

    [[nodiscard]] bool in_bounds(const Vector2i& pos) const
    {
        return pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y;
    }

    void print() const
    {
        for (int y = 0; y < size.y; ++y) {
            for (int x = 0; x < size.x; ++x) {
                const GridState state = at({ x, y });
                if (start == Vector2i { x, y }) {
                    std::print("S");
                } else if (state == GridState::empty) {
                    std::print(".");
                } else if (state == GridState::splitter) {
                    std::print("^");
                } else {
                    assert(false);
                }
            }
            std::println();
        }
    }
};

static uint64_t count_timelines(
    const Grid& grid, const Vector2i start, std::unordered_map<Vector2i, uint64_t, Vector2i::Hash>& memos)
{
    if (const auto it = memos.find(start); it != memos.end()) {
        return it->second;
    }
    for (int y = start.y; y < grid.size.y; ++y) {
        if (grid.at({ start.x, y }) == GridState::splitter) {
            const Vector2i left { start.x - 1, y };
            const Vector2i right { start.x + 1, y };
            assert(grid.in_bounds(left) && grid.in_bounds(right));
            assert(grid.at(left) == GridState::empty && grid.at(right) == GridState::empty);
            const uint64_t count = count_timelines(grid, left, memos) + count_timelines(grid, right, memos);
            memos[start] = count;
            return count;
        }
    }
    constexpr uint64_t count = 1;
    memos[start] = count;
    return count;
}

static Grid parse_grid(const std::string& data)
{
    std::vector<GridState> grid_data;
    std::optional<Vector2i> start;
    std::optional<int> width;
    for (int pos = 0; pos < data.size(); ++pos) {
        if (const char c = data[pos]; c == '\n') {
            if (!width.has_value()) {
                width = pos;
            } else {
                assert(grid_data.size() % *width == 0);
            }
        } else if (c == '.') {
            grid_data.emplace_back(GridState::empty);
        } else if (c == 'S') {
            assert(!start.has_value() && !width.has_value());
            start = { pos, 0 };
            grid_data.emplace_back(GridState::empty);
        } else if (c == '^') {
            grid_data.emplace_back(GridState::splitter);
        } else {
            assert(false);
        }
    }
    assert(start.has_value());
    assert(width.has_value());
    assert(grid_data.size() % *width == 0);
    return { .size = { *width, static_cast<int>(grid_data.size()) / *width },
             .start = *start,
             .data = std::move(grid_data) };
}

static uint64_t solve(const std::string& data)
{
    const Grid grid = parse_grid(data);
    std::unordered_map<Vector2i, uint64_t, Vector2i::Hash> memos;
    return count_timelines(grid, grid.start, memos);
}

int main()
{
    const std::string data = read_file("./day07-part2/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 10000);
#else
    std::println("{}", solve(data));
#endif
}