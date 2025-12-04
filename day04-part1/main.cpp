#include <utils.hpp>

#include <array>
#include <print>

struct Vector2i {
    int x;
    int y;

    Vector2i operator+(const Vector2i& other) const
    {
        return { x + other.x, y + other.y };
    }
};

struct Grid {
    enum State : uint8_t { empty, roll };

    Vector2i size;
    std::vector<State> data;

    State& at(const Vector2i& pos)
    {
        return data[pos.x + size.x * pos.y];
    }

    [[nodiscard]] const State& at(const Vector2i& pos) const
    {
        return data[pos.x + size.x * pos.y];
    }

    [[nodiscard]] bool in_bounds(const Vector2i& pos) const
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

static int count_accessible(const Grid& grid)
{
    int count = 0;
    for (int y = 0; y < grid.size.y; ++y) {
        for (int x = 0; x < grid.size.x; ++x) {
            constexpr std::array<Vector2i, 8> offsets {
                { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } }
            };
            const Vector2i current { x, y };
            if (grid.at(current) != Grid::State::roll) {
                continue;
            }
            uint8_t neighbor_count = 0;
            for (const Vector2i& offset : offsets) {
                if (const Vector2i neighbor = current + offset;
                    grid.in_bounds(neighbor) && grid.at(neighbor) == Grid::State::roll) {
                    ++neighbor_count;
                }
            }
            if (neighbor_count < 4) {
                ++count;
            }
        }
    }
    return count;
}

static int solve(const std::string& data)
{
    const Grid grid = parse_grid(data);
    return count_accessible(grid);
}

int main()
{
    const std::string data = read_file("./day04-part1/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 1000);
#else
    std::println("{}", solve(data));
#endif
}