#pragma once
#include <array>
#include <string>
#include <shared_mutex>

class MyStruct {
private:
    static constexpr int M = 3;

    std::array<int, M> data{0, 0, 0};
    mutable std::array<std::shared_mutex, M> mx;

    mutable std::shared_mutex cache_mx;
    mutable std::string cached;
    mutable bool cache_valid{false};

    void rebuild_cache() const;

public:
    int  read(int i) const;
    void write(int i, int val);
    operator std::string() const;
};
