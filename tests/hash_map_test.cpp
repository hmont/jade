#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include "hash_map.hpp"

namespace {

int failures = 0;

void expect(bool condition, std::string_view description) {
    if (!condition) {
        std::cerr << "FAILED: " << description << '\n';
        ++failures;
    }
}

void expect_value(
    const HashMap& map,
    variant key,
    variant expected,
    std::string_view description
) {
    const std::optional<variant> actual = map.get(std::move(key));
    expect(actual.has_value(), description);

    if (actual.has_value()) {
        expect(*actual == expected, description);
    }
}

void test_set_get_update_and_remove() {
    HashMap map;

    expect(!map.get("missing").has_value(), "a missing key returns no value");

    map.set("language", "C++");
    expect_value(map, "language", "C++", "set stores a value");

    map.set("language", 20);
    expect_value(map, "language", 20, "set replaces an existing value");

    map.remove("language");
    expect(!map.get("language").has_value(), "remove erases an existing key");

    map.remove("language");
    expect(!map.get("language").has_value(), "removing a missing key is harmless");
}

void test_collisions_and_key_types() {
    HashMap map(1);

    map.set("one", 1);
    map.set(7, "integer");
    map.set(7.0, "double");

    expect_value(map, "one", 1, "a string key survives a collision");
    expect_value(map, 7, "integer", "an integer key survives a collision");
    expect_value(map, 7.0, "double", "key types remain distinct");

    map.remove(7);
    expect(!map.get(7).has_value(), "the selected colliding key is removed");
    expect_value(map, "one", 1, "removing a collision preserves earlier entries");
    expect_value(map, 7.0, "double", "removing a collision preserves later entries");
}

void test_bucket_count_and_stream_output() {
    HashMap map(0);
    expect(map.buckets.size() == 1, "a zero bucket count is normalized to one");

    map.set("alpha", 1);
    map.set(2, "two");

    std::ostringstream output;
    output << map;
    expect(
        output.str() == R"({"alpha": 1, 2: "two"})",
        "stream output formats entries in the bucket"
    );
}

}  // namespace

int main() {
    test_set_get_update_and_remove();
    test_collisions_and_key_types();
    test_bucket_count_and_stream_output();

    if (failures != 0) {
        std::cerr << failures << " assertion(s) failed\n";
        return 1;
    }

    std::cout << "All HashMap tests passed\n";
    return 0;
}
