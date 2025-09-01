#ifndef STORE_HPP
#define STORE_HPP

#include <string>
#include <unordered_map>
#include <optional>

class Store {
public:
    void put(const std::string &key, const std::string &value);
    std::optional<std::string> get(const std::string &key) const;
    bool update(const std::string &key, const std::string &value);
    bool remove(const std::string &key);
    std::optional<std::string> getKey(const std::string &value) const;
    size_t size() const;

private:
    std::unordered_map<std::string, std::string> data;
};

#endif
