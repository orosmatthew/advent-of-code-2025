#include <utils.hpp>

#include <raylib-cpp.hpp>

#include <array>

namespace rl = raylib;

struct Vector2i {
    int x;
    int y;

    Vector2i operator+(const Vector2i& other) const
    {
        return { x + other.x, y + other.y };
    }

    bool operator==(const Vector2i& other) const
    {
        return x == other.x && y == other.y;
    }
};

struct Grid {
    enum State : uint8_t { empty, roll };

    int size {};
    std::vector<State> data;

    State& at(const Vector2i& pos)
    {
        return data[pos.x + size * pos.y];
    }

    [[nodiscard]] const State& at(const Vector2i& pos) const
    {
        return data[pos.x + size * pos.y];
    }

    [[nodiscard]] bool in_bounds(const Vector2i& pos) const
    {
        return pos.x >= 0 && pos.x < size && pos.y >= 0 && pos.y < size;
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
    assert(grid_data.size() / *width == width);
    return Grid { .size = *width, .data = std::move(grid_data) };
}

static bool accessible(const Grid& grid, const Vector2i& pos)
{
    assert(grid.in_bounds(pos));
    assert(grid.at(pos) == Grid::State::roll);
    constexpr std::array<Vector2i, 8> offsets {
        { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } }
    };
    uint8_t neighbor_count = 0;
    for (const Vector2i& offset : offsets) {
        if (const Vector2i neighbor = pos + offset;
            grid.in_bounds(neighbor) && grid.at(neighbor) == Grid::State::roll) {
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
    for (int y = 0; y < grid.size; ++y) {
        for (int x = 0; x < grid.size; ++x) {
            const Vector2i pos { x, y };
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
    assert(output.data.size() == output.size * output.size);
    return removed;
}

static void update_grid_image(const Grid& grid, rl::Image& image)
{
    assert(image.GetWidth() == grid.size && image.GetHeight() == grid.size);
    for (int y = 0; y < grid.size; ++y) {
        for (int x = 0; x < grid.size; ++x) {
            if (grid.at({ x, y }) == Grid::State::roll) {
                image.DrawPixel(x, y, RED);
            } else {
                rl::Color color = image.GetColor(x, y);
                color = color.Lerp(BLACK, 0.7f);
                image.DrawPixel(x, y, color);
            }
        }
    }
}

static rl::Rectangle calc_grid_rect(const rl::Window& window)
{
    const rl::Vector2 window_size = window.GetSize();
    const float size = std::min(window_size.x, window_size.y);
    const rl::Vector2 pos { (window_size.x - size) / 2.0f, (window_size.y - size) / 2.0f };
    return { pos, { size, size } };
}

static void draw_border(const rl::Window& window)
{
    const rl::Rectangle grid_rect = calc_grid_rect(window);
    constexpr float border_thickness = 4.0f;
    const rl::Rectangle border_rect {
        grid_rect.GetPosition() - rl::Vector2 { border_thickness, border_thickness },
        rl::Vector2 { grid_rect.GetWidth() + border_thickness * 2, grid_rect.GetHeight() + border_thickness * 2 }
    };
    border_rect.DrawLines(DARKGRAY, border_thickness);
}

static void draw_grid(const rl::Window& window, const rl::Texture& grid_texture)
{
    const rl::Rectangle grid_rect = calc_grid_rect(window);
    const rl::Rectangle grid_rect_src {
        0.0f, 0.0f, static_cast<float>(grid_texture.GetWidth()), static_cast<float>(grid_texture.GetHeight())
    };
    grid_texture.Draw(grid_rect_src, grid_rect);
}

int main()
{
    const std::string data = read_file("./day04-part2-visualization/input.txt");
    const rl::Window window(800, 800, "AOC 2025 | Day 4 Part 2", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    Grid grid = parse_grid(data);
    Grid grid_output;
    rl::Image grid_image { grid.size, grid.size, BLACK };
    update_grid_image(grid, grid_image);
    rl::Texture grid_texture { grid_image };
    constexpr double step_seconds = 0.5;
    double next_time = GetTime() + step_seconds;

    auto update_grid = [&] {
        remove_accessible_rolls(grid, grid_output);
        std::swap(grid, grid_output);
        update_grid_image(grid, grid_image);
        grid_texture.Update(grid_image.data);
        next_time = GetTime() + step_seconds;
    };

    auto reset_grid = [&] {
        grid = parse_grid(data);
        update_grid_image(grid, grid_image);
        grid_texture.Update(grid_image.data);
        next_time = GetTime() + step_seconds;
    };

    while (!window.ShouldClose()) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_R)) {
            reset_grid();
        }
        if (const double now = GetTime(); now >= next_time) {
            update_grid();
        }
        BeginDrawing();
        ClearBackground(BLACK);
        draw_border(window);
        draw_grid(window, grid_texture);
        EndDrawing();
    }
}