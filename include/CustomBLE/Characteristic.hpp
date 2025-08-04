#pragma once
#include <string>
#include <functional>
#include <cstring>
#include <esp_log.h>
#include <host/ble_gatt.h>
#include <host/ble_uuid.h>
#include <host/ble_hs.h>

namespace CustomBLE {

class Characteristic {
public:
    using ReadCallback = std::function<std::string()>;
    using WriteCallback = std::function<void(const std::string&)>;

    Characteristic(const ble_uuid128_t& characteristic_uuid, 
                  const std::string& initial_value,
                  ReadCallback read_cb = nullptr,
                  WriteCallback write_cb = nullptr);

    int handle_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt);
    static int gatt_access_callback(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt, void *arg);

    const ble_uuid_t* get_uuid() const;
    uint16_t get_flags() const;
    void set_handle(uint16_t char_handle);
    uint16_t get_handle() const;
    void set_read_callback(ReadCallback callback);
    void set_write_callback(WriteCallback callback);

    /**
     * @brief Generate a string overview of the characteristic.
     */
    std::string overview() const;

    /**
     * @brief Print the overview using printf().
     */
    void print() const;

    // Static factory methods for pointer-based characteristics
    template<typename T>
    static ReadCallback make_pointer_read_callback(T* value_ptr) {
        return [value_ptr]() {
            return std::string(reinterpret_cast<const char*>(value_ptr), sizeof(T));
        };
    }

    template<typename T>
    static WriteCallback make_pointer_write_callback(T* value_ptr) {
        return [value_ptr](const std::string& data) {
            if (data.size() == sizeof(T)) {
                std::memcpy(value_ptr, data.data(), sizeof(T));
            }
        };
    }

    // Static factory methods for creating complete characteristics from pointers
    template<typename T>
    static Characteristic from_pointer_read_only(const ble_uuid128_t& uuid, T* value_ptr) {
        auto read_cb = make_pointer_read_callback(value_ptr);
        return Characteristic(uuid, "", read_cb, nullptr);
    }

    template<typename T>
    static Characteristic from_pointer_read_write(const ble_uuid128_t& uuid, T* value_ptr) {
        auto read_cb = make_pointer_read_callback(value_ptr);
        auto write_cb = make_pointer_write_callback(value_ptr);
        return Characteristic(uuid, "", read_cb, write_cb);
    }

    template<typename T>
    static Characteristic from_pointer_write_only(const ble_uuid128_t& uuid, T* value_ptr) {
        auto write_cb = make_pointer_write_callback(value_ptr);
        return Characteristic(uuid, "", nullptr, write_cb);
    }
    
    // Static factory method for fixed value (read-only) characteristics
    static Characteristic from_fixed_value(const ble_uuid128_t& uuid, const std::string& value) {
        ReadCallback read_cb = [value]() { return value; };
        return Characteristic(uuid, value, read_cb, nullptr);
    }

private:
    ble_uuid128_t uuid;
    uint16_t handle;
    ReadCallback read_callback;
    WriteCallback write_callback;
    std::string default_value;
    uint16_t flags;
};

} // namespace CustomBLE
