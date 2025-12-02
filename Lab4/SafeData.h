#pragma once
#include <vector>
#include <string>
#include <shared_mutex>
#include <mutex>
#include <sstream>

class SafeData
{
private:
    int data[3] = {1, 1, 1};
    mutable std::shared_mutex mutexes[3];

public:
    SafeData() = default;

    SafeData(const SafeData&) = delete;
    SafeData& operator=(const SafeData&) = delete;

    int read(int index) const
    {
        if (index < 0 || index >= 3) return -1;
        std::shared_lock lock(mutexes[index]);
        return data[index];
    }

    void write(int index, int value)
    {
        if (index < 0 || index >= 3) return;
        std::unique_lock lock(mutexes[index]);
        data[index] = value;
    }

    operator std::string() const
    {
        std::shared_lock<std::shared_mutex> l0(mutexes[0], std::defer_lock);
        std::shared_lock<std::shared_mutex> l1(mutexes[1], std::defer_lock);
        std::shared_lock<std::shared_mutex> l2(mutexes[2], std::defer_lock);

        std::lock(l0, l1, l2);

        std::stringstream ss;
        ss << "[" << data[0] << ", " << data[1] << ", " << data[2] << "]";
        return ss.str();
    }
};
