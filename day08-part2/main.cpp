#include <utils.hpp>

#include <algorithm>
#include <print>
#include <ranges>
#include <unordered_map>

struct Vector3u64 {
    uint64_t x { 0 };
    uint64_t y { 0 };
    uint64_t z { 0 };

    [[nodiscard]] uint64_t distance_sqrd(const Vector3u64& other) const
    {
        const auto [dx, dy, dz] = other - *this;
        return dx * dx + dy * dy + dz * dz;
    }

    [[nodiscard]] bool operator==(const Vector3u64& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    [[nodiscard]] Vector3u64 operator-(const Vector3u64& other) const
    {
        return { x - other.x, y - other.y, z - other.z };
    }

    [[nodiscard]] bool operator<(const Vector3u64& other) const
    {
        if (x != other.x) {
            return x < other.x;
        }
        if (y != other.y) {
            return y < other.y;
        }
        return z < other.z;
    }

    struct Hash {
        size_t operator()(const Vector3u64& vector) const noexcept
        {
            return hash_combine(vector.x, hash_combine(vector.y, vector.z));
        }
    };
};

static std::vector<Vector3u64> parse_positions(const std::string& data)
{
    std::vector<Vector3u64> positions;
    for (int pos = 0; pos < data.size(); ++pos) {
        const auto x = parse_uint<uint64_t>(data, pos);
        assert(data[pos] == ',');
        ++pos;
        const auto y = parse_uint<uint64_t>(data, pos);
        assert(data[pos] == ',');
        ++pos;
        const auto z = parse_uint<uint64_t>(data, pos);
        assert(data[pos] == '\n');
        positions.emplace_back<Vector3u64>({ x, y, z });
    }
    return positions;
}

struct JunctionPair {
    Vector3u64 first;
    Vector3u64 second;
    uint64_t distance_sqrd;

    [[nodiscard]] bool operator==(const JunctionPair& other) const
    {
        return first == other.first && second == other.second && distance_sqrd == other.distance_sqrd;
    }
};

static std::vector<JunctionPair> create_sorted_pairs(const std::vector<Vector3u64>& positions)
{
    thread_local std::vector<JunctionPair> pairs;
    pairs.clear();
    for (size_t i = 0; i < positions.size(); ++i) {
        for (size_t j = i + 1; j < positions.size(); ++j) {
            const Vector3u64 first = positions[i];
            const Vector3u64 second = positions[j];
            assert(first != second);
            JunctionPair pair { first, second, first.distance_sqrd(second) };
            if (pair.second < pair.first) {
                std::swap(pair.first, pair.second);
            }
            // Should assert that pair isn't already in pairs, but it is too slow.
            // The final answer is the same without the check so the input is probably
            // specially crafted to not have duplicates.
            pairs.emplace_back(pair);
        }
    }
    std::ranges::sort(pairs, [](const auto& a, auto& b) { return a.distance_sqrd < b.distance_sqrd; });
    return pairs;
}

using CircuitId = uint64_t;

static std::optional<JunctionPair> get_last_pair_to_fully_connect(
    const std::vector<Vector3u64>& positions, const std::vector<JunctionPair>& pairs)
{
    std::unordered_map<Vector3u64, CircuitId, Vector3u64::Hash> circuits;
    CircuitId circuit_id_count = 0;
    for (const Vector3u64& pos : positions) {
        circuits[pos] = circuit_id_count;
        ++circuit_id_count;
    }
    std::optional<JunctionPair> last_pair;
    for (const auto& pair : pairs) {
        const auto first_it = circuits.find(pair.first);
        const auto second_it = circuits.find(pair.second);
        assert(first_it != circuits.end() && second_it != circuits.end());
        const CircuitId first_id = first_it->second;
        const CircuitId second_id = second_it->second;
        if (first_id == second_id) {
            continue;
        }
        const CircuitId new_id = circuit_id_count;
        ++circuit_id_count;
        for (auto& id : circuits | std::views::values) {
            if (id == first_id || id == second_id) {
                id = new_id;
            }
        }
        const CircuitId check_id = circuits.begin()->second;
        if (std::ranges::all_of(circuits, [check_id](const auto& connection) {
                return connection.second == check_id;
            })) {
            last_pair = pair;
            break;
        }
    }
    return last_pair;
}

static uint64_t solve(const std::string& data)
{
    const std::vector<Vector3u64> positions = parse_positions(data);
    const std::vector<JunctionPair> pairs = create_sorted_pairs(positions);
    const std::optional<JunctionPair> last_pair = get_last_pair_to_fully_connect(positions, pairs);
    assert(last_pair.has_value());
    return last_pair->first.x * last_pair->second.x;
}

int main()
{
    const std::string data = read_file("./day08-part2/input.txt");
#ifdef BENCHMARK
    benchmark([&] { return solve(data); }, 100);
#else
    std::println("{}", solve(data));
#endif
}