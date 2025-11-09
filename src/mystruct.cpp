#include "mystruct.h"
#include <mutex>
#include <shared_mutex>
#include <array>
#include <string>

int MyStruct::read(int i) const {
    std::shared_lock<std::shared_mutex> lk(mx[i]);
    return data[i];
}

void MyStruct::write(int i, int val) {
    {
        std::unique_lock<std::shared_mutex> lk(mx[i]);
        data[i] = val;
    }
    {
        std::unique_lock<std::shared_mutex> lk(cache_mx);
        cache_valid = false;
    }
}

void MyStruct::rebuild_cache() const {
    std::shared_lock<std::shared_mutex> a(mx[0]);
    std::shared_lock<std::shared_mutex> b(mx[1]);
    std::shared_lock<std::shared_mutex> c(mx[2]);

    cached = std::string("{" + std::to_string(data[0]) + "," + std::to_string(data[1]) + "," + std::to_string(data[2]) + "}");
    cache_valid = true;
}

MyStruct::operator std::string() const {
    {
        std::shared_lock<std::shared_mutex> r(cache_mx);
        if (cache_valid) return cached;
    }
    {
        std::unique_lock<std::shared_mutex> w(cache_mx);
        if (!cache_valid) {
            rebuild_cache();
        }
        return cached;
    }
}
