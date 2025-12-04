#include <utils.hpp>

#include <array>
#include <print>

struct Vector2 {
    int x;
    int y;

    Vector2 operator+(const Vector2& other) const
    {
        return { x + other.x, y + other.y };
    }

    bool operator==(const Vector2& other) const
    {
        return x == other.x && y == other.y;
    }
};

struct Grid {
    enum State : uint8_t { empty, roll };

    Vector2 size {};
    std::vector<State> data;

    State& at(const Vector2& pos)
    {
        return data[pos.x + size.x * pos.y];
    }

    [[nodiscard]] const State& at(const Vector2& pos) const
    {
        return data[pos.x + size.x * pos.y];
    }

    [[nodiscard]] bool in_bounds(const Vector2& pos) const
    {
        return pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y;
    }
};

static Grid parse_grid(const std::string& data)
{
    std::optional<int> width;
    std::vector<Grid::State> grid_data;
    for (int pos = 0; pos < data.size(); ++pos) {
        if (data[pos] == '\n' || pos >= data.size()) {
            if (!width.has_value()) {
                width = pos;
            }
        } else {
            switch (data[pos]) {
            case '.':
                grid_data.push_back(Grid::State::empty);
                break;
            case '@':
                grid_data.push_back(Grid::State::roll);
                break;
            default:
                assert(false);
            }
        }
    }
    assert(width.has_value());
    assert(grid_data.size() % *width == 0);
    return Grid { .size = { *width, static_cast<int>(grid_data.size()) / *width }, .data = std::move(grid_data) };
}

static bool accessible(const Grid& grid, const Vector2& pos)
{
    assert(grid.in_bounds(pos));
    assert(grid.at(pos) == Grid::State::roll);
    constexpr std::array<Vector2, 8> offsets {
        { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } }
    };
    uint8_t neighbor_count = 0;
    for (const Vector2& offset : offsets) {
        if (const Vector2 neighbor = pos + offset; grid.in_bounds(neighbor) && grid.at(neighbor) == Grid::State::roll) {
            ++neighbor_count;
        }
    }
    if (neighbor_count >= 4) {
        return false;
    }
    return true;
}

static int remove_accessible_rolls(const Grid& grid, Grid& output)
{
    output.size = grid.size;
    output.data.clear();
    int removed = 0;
    for (int y = 0; y < grid.size.y; ++y) {
        for (int x = 0; x < grid.size.x; ++x) {
            const Vector2 pos { x, y };
            if (grid.at(pos) == Grid::State::empty) {
                output.data.emplace_back(Grid::State::empty);
                continue;
            }
            if (accessible(grid, pos)) {
                output.data.emplace_back(Grid::State::empty);
                ++removed;
                continue;
            }
            output.data.emplace_back(Grid::State::roll);
        }
    }
    assert(output.data.size() == output.size.x * output.size.y);
    return removed;
}

static int solve(const std::string& data)
{
    Grid grid = parse_grid(data);
    Grid output;
    int total_removed = 0;
    while (true) {
        const int removed = remove_accessible_rolls(grid, output);
        if (removed == 0) {
            break;
        }
        total_removed += removed;
        std::swap(grid, output);
    }
    return total_removed;
}

int main()
{
    const std::string data = read_file("./day04-part2/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 1000);
#else
    std::println("{}", solve(data));
#endif
}