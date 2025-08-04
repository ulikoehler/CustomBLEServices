#pragma once
#include <string>
#include <functional>
#include <string>
#include <functional>
#include <cstring>
template<typename T>
std::function<std::string()> make_pointer_read_callback(T* value_ptr) {
    return [value_ptr]() {
        return std::string(reinterpret_cast<const char*>(value_ptr), sizeof(T));
    };
}

// Write callback for a pointer to any type (sets value from binary string)
template<typename T>
std::function<void(const std::string&)> make_pointer_write_callback(T* value_ptr) {
    return [value_ptr](const std::string& data) {
        if (data.size() == sizeof(T)) {
            std::memcpy(value_ptr, data.data(), sizeof(T));
        }
        // else: ignore or handle error
    };
}

// Read callback for a fixed value (returns the value)
inline std::function<std::string()> make_fixed_read_callback(const std::string& value) {
    return [value]() { return value; };
}

// Write callback for a fixed value (updates the value)
inline std::function<void(const std::string&)> make_fixed_write_callback(std::string& value) {
    return [&value](const std::string& data) { value = data; };
}
