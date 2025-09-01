#include "Store.hpp"

void Store::put(const std::string &key, const std::string &value) {
    data[key] = value;
}

std::optional<std::string> Store::get(const std::string &key) const {
    auto it = data.find(key);
    if (it != data.end()) return it->second;
    return std::nullopt;
}

bool Store::update(const std::string &key, const std::string &value) {
    auto it = data.find(key);
    if (it != data.end()) {
        it->second = value;
        return true;
    }
    return false;
}

bool Store::remove(const std::string &key) {
    return data.erase(key) > 0;
}

std::optional<std::string> Store::getKey(const std::string &value) const {
    for (const auto &pair : data) {
        if (pair.second == value) return pair.first;
    }
    return std::nullopt;
}

size_t Store::size() const {
    return data.size();
}
