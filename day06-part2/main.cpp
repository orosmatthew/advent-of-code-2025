#include <utils.hpp>

#include <print>

struct Vector2i {
    int x;
    int y;
};

struct Grid {
    Vector2i size;
    std::vector<std::optional<uint8_t>> data;

    [[nodiscard]] const std::optional<uint8_t>& at(const Vector2i& pos) const
    {
        return data[pos.x + size.x * pos.y];
    }
};

enum class OpType { add, multiply };

struct Op {
    OpType type;
    int start;
    int end;
};

static uint64_t push_digit(const uint64_t num, const uint8_t digit)
{
    return num * 10 + digit;
}

static Grid parse_digits(const std::string& data, int& pos)
{
    std::vector<std::optional<uint8_t>> digits;
    std::optional<int> width;
    for (; pos < data.size(); ++pos) {
        if (data[pos] == '*' || data[pos] == '+') {
            break;
        }
        if (data[pos] == '\n') {
            if (!width.has_value()) {
                width = static_cast<int>(digits.size());
            } else {
                assert(digits.size() % *width == 0);
            }
            continue;
        }
        if (data[pos] == ' ') {
            digits.emplace_back(std::nullopt);
        } else {
            assert(is_digit(data[pos]));
            digits.emplace_back(data[pos] - '0');
        }
    }
    assert(width.has_value());
    assert(digits.size() % *width == 0);
    return Grid { .size = { *width, static_cast<int>(digits.size()) / *width }, .data = std::move(digits) };
}

static std::vector<Op> parse_ops(const std::string& data, int& pos)
{
    std::vector<Op> ops;
    std::optional<Op> op;
    int count = 0;
    for (; pos < data.size(); ++pos) {
        if (const char c = data[pos]; c == '\n') {
            if (op.has_value()) {
                op->start = count - 1;
                ops.emplace_back(*op);
            }
        } else if (c == '+' || c == '*') {
            if (op.has_value()) {
                op->start = count - 2;
                ops.emplace_back(*op);
            }
            const OpType type = c == '+' ? OpType::add : OpType::multiply;
            op = { .type = type, .start = count - 1, .end = count - 1 };
        } else if (c != ' ') {
            assert(false);
        }
        ++count;
    }
    return ops;
}

static uint64_t solve(const std::string& data)
{
    int pos = 0;
    const Grid grid = parse_digits(data, pos);
    uint64_t total = 0;
    for (const std::vector<Op> ops = parse_ops(data, pos); const auto [type, start, end] : ops) {
        uint64_t result = type == OpType::add ? 0 : 1;
        for (int x = start; x > end; --x) {
            uint64_t num = 0;
            for (int y = 0; y < grid.size.y; ++y) {
                if (std::optional<uint8_t> digit = grid.at({ x, y }); digit.has_value()) {
                    num = push_digit(num, *digit);
                }
            }
            if (type == OpType::add) {
                result += num;
            } else {
                result *= num;
            }
        }
        total += result;
    }

    return total;
}

int main()
{
    const std::string data = read_file("./day06-part2/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 100000);
#else
    std::println("{}", solve(data));
#endif
}