#ifndef STORE_HPP
#define STORE_HPP

#include <string>
#include <unordered_map>

class Store {
public:
    void put(const std::string &key, const std::string &value);
    std::string get(const std::string &key);
    void update(const std::string &key, const std::string &value);
    void remove(const std::string &key);
    std::string getKey(const std::string &value);
    std::string stats() const;
    int32_t count_size(); // returns size of kv

private:
    std::unordered_map<std::string, std::string> kv;
};

#endif
