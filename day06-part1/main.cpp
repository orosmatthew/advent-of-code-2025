#include <utils.hpp>

#include <print>

struct Vector2i {
    int x;
    int y;
};

struct Grid {
    Vector2i size;
    std::vector<uint64_t> data;

    [[nodiscard]] const uint64_t& at(const Vector2i& pos) const
    {
        return data[pos.x + size.x * pos.y];
    }
};

static void skip_spaces(const std::string& data, int& pos)
{
    while (data[pos] == ' ') {
        ++pos;
    }
}

static Grid parse_numbers(const std::string& data, int& pos)
{
    std::vector<uint64_t> numbers;
    std::optional<int> width;
    while (pos < data.size()) {
        skip_spaces(data, pos);
        if (data[pos] == '*' || data[pos] == '+') {
            break;
        }
        if (data[pos] == '\n') {
            if (!width.has_value()) {
                width = static_cast<int>(numbers.size());
            } else {
                assert(numbers.size() % *width == 0);
            }
            ++pos;
            continue;
        }
        skip_spaces(data, pos);
        numbers.emplace_back(parse_uint<uint64_t>(data, pos));
    }
    assert(width.has_value());
    assert(numbers.size() % *width == 0);
    return Grid { .size = { *width, static_cast<int>(numbers.size()) / *width }, .data = std::move(numbers) };
}

enum class Op { add, multiply };

static uint64_t solve(const std::string& data)
{
    int pos = 0;
    const Grid numbers = parse_numbers(data, pos);
    int col_count = 0;
    uint64_t total = 0;
    for (; pos < data.size(); ++pos) {
        skip_spaces(data, pos);
        std::optional<Op> op;
        switch (data[pos]) {
        case '*':
            op = Op::multiply;
            break;
        case '+':
            op = Op::add;
            break;
        case '\n':
            break;
        default:
            assert(false);
        }
        if (!op.has_value()) {
            break;
        }
        uint64_t result = *op == Op::add ? 0 : 1;
        for (int y = 0; y < numbers.size.y; ++y) {
            const uint64_t value = numbers.at({ col_count, y });
            if (*op == Op::add) {
                result += value;
            } else {
                result *= value;
            }
        }
        total += result;
        ++col_count;
    }
    return total;
}

int main()
{
    const std::string data = read_file("./day06-part1/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 100000);
#else
    std::println("{}", solve(data));
#endif
}