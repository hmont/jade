#pragma once

#include <list>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <optional>
#include <ostream>

typedef std::variant<std::string, double, int> variant;
typedef std::pair<variant, variant> entry;

class HashMap {
public:
    std::vector<std::list<entry>> buckets;
    std::hash<variant> hash_function;

    explicit HashMap(std::size_t = 1024);
    void set(variant, variant);
    std::optional<variant> get(variant) const;
    void remove(variant);

    friend std::ostream& operator<<(std::ostream&, const HashMap&);
};

inline bool replace(
    std::list<entry>& bucket,
    variant& key,
    variant& value
) {
    for (entry& e : bucket) {
        if (e.first == key) {
            e.second = value;
            return true;
        }
    }

    return false;
}

inline std::ostream& operator<<(std::ostream& os, const variant& v) {
    std::visit([&os](const auto& arg) {
        if (std::is_same_v<std::string, std::decay_t<decltype(arg)>>) {
            os << '"' << arg << '"';
        } else {
            os << arg;
        }
    }, v);

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const entry& e) {
    os << e.first << ": " << e.second;
    return os;
}

inline bool operator==(const variant& v1, const variant& v2) {
    return std::visit([](const auto& arg1, const auto& arg2) {
        using A = std::decay_t<decltype(arg1)>;
        using B = std::decay_t<decltype(arg2)>;

        if constexpr (!std::is_same_v<A, B>) {
            return false;
        } else if constexpr (std::is_floating_point_v<A>) {
            return arg1 == arg2 || (std::isnan(arg1) && std::isnan(arg2));
        } else {
            return arg1 == arg2;
        }
    }, v1, v2);
}


HashMap::HashMap(std::size_t buckets) : buckets(buckets) {
    if (buckets < 1) this->buckets.resize(1);
    this->hash_function = std::hash<variant>();
}

std::ostream& operator<<(std::ostream& os, const HashMap& map) {
    os << "{";

    bool first = true;

    for (std::list<entry> list : map.buckets) {
        for (entry e : list) {
            if (first) {
                first = !first;
            } else {
                os << ", ";
            }

            os << e;
        }
    }

    os << "}";

    return os;
}

inline void HashMap::set(variant key, variant value) {
    std::size_t index = this->hash_function(key) % this->buckets.size();

    std::list<entry>& bucket = this->buckets[index];

    if (!replace(bucket, key, value)) {
        bucket.push_back({key, value});
    }
}

inline std::optional<variant> HashMap::get(variant key) const {
    std::size_t index = this->hash_function(key) % this->buckets.size();

    for (const entry& e : this->buckets[index]) {
        if (e.first == key) return e.second;
    }

    return std::nullopt;
}

inline void HashMap::remove(variant key) {
    std::size_t index = this->hash_function(key) % this->buckets.size();

    std::list<entry>& bucket = this->buckets[index];

    auto iter = bucket.begin();

    while (iter != bucket.end() && iter->first != key) {
        iter++;
    }

    if (iter != bucket.end()) {
        bucket.erase(iter);
    }
}