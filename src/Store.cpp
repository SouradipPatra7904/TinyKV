#include "Store.hpp"

void Store::put(const std::string &key, const std::string &value) {
    kv[key] = value;
}

std::string Store::get(const std::string &key) {
    if (kv.find(key) != kv.end()) {
        return kv[key];
    }
    return "NOT_FOUND";
}

void Store::update(const std::string &key, const std::string &value) {
    kv[key] = value;
}

void Store::remove(const std::string &key) {
    kv.erase(key);
}

std::string Store::getKey(const std::string &value) {
    for (const auto &pair : kv) {
        if (pair.second == value) {
            return pair.first;
        }
    }
    return "NOT_FOUND";
}

std::string Store::stats() const {
    return "Number of keys: " + std::to_string(kv.size());
}

int32_t Store::count_size()
{
    return kv.size();
}
