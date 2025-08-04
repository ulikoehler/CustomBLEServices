#pragma once

#include <string>


/**
 * @brief Converts any datatype to a string containing its raw binary content.
 *
 * This template function takes a value of any type and returns a std::string
 * of size sizeof(T), containing the raw binary bytes of the value.
 *
 * @tparam T The type of the value to convert (e.g., int, uint8_t, float, struct, etc.).
 * @param value The value to be converted to a binary string.
 * @return std::string A string containing the raw binary representation of the value.
 *
 * @note The returned string contains non-printable characters and is not human-readable.
 *       Useful for serialization, communication, or storage of binary data.
 *
 * @example
 *   int x = 5;
 *   std::string bin = ToBinaryString(x); // bin.size() == sizeof(int), contains raw bytes
 */
template <typename T>
std::string ToBinaryString(const T& value) {
    return std::string(reinterpret_cast<const char*>(&value), sizeof(T));
}