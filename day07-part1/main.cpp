#include <utils.hpp>

#include <print>

struct Vector2i {
    int x;
    int y;

    bool operator==(const Vector2i& other) const
    {
        return x == other.x && y == other.y;
    }
};

enum class GridState : uint8_t { empty, splitter, beam };

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
                } else if (state == GridState::beam) {
                    std::print("|");
                } else {
                    assert(false);
                }
            }
            std::println();
        }
    }
};

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
            grid_data.emplace_back(GridState::beam);
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

static int solve(const std::string& data)
{
    Grid grid = parse_grid(data);
    int split_count = 0;
    for (int y = 1; y < grid.size.y; ++y) {
        for (int x = 0; x < grid.size.x; ++x) {
            const GridState state = grid.at({ x, y });
            const GridState above = grid.at({ x, y - 1 });
            if (state == GridState::empty && above == GridState::beam) {
                grid.at({ x, y }) = GridState::beam;
                continue;
            }
            if (state == GridState::splitter && above == GridState::beam) {
                const Vector2i left { x - 1, y };
                const Vector2i right { x + 1, y };
                assert(grid.in_bounds(left) && grid.in_bounds(right));
                assert(grid.at(left) != GridState::splitter && grid.at(right) != GridState::splitter);
                grid.at(left) = GridState::beam;
                grid.at(right) = GridState::beam;
                ++split_count;
            }
        }
    }
    return split_count;
}

int main()
{
    const std::string data = read_file("./day07-part1/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 100000);
#else
    std::println("{}", solve(data));
#endif
}