    /**
     * @brief Generate a string overview of the characteristic.
     */
    std::string overview() const;

    /**
     * @brief Print the overview using printf().
     */
    void print() const;

#pragma once
#include <string>
#include <functional>
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

private:
    ble_uuid128_t uuid;
    uint16_t handle;
    ReadCallback read_callback;
    WriteCallback write_callback;
    std::string default_value;
    uint16_t flags;
};

} // namespace CustomBLE

public:
    /**
     * @brief Construct a new CustomBLECharacteristic object.
     * @param characteristic_uuid 128-bit UUID for the characteristic
     * @param initial_value Initial value for the characteristic
     * @param read_cb Optional read callback function
     * @param write_cb Optional write callback function
     */
    CustomBLECharacteristic(const ble_uuid128_t& characteristic_uuid, 
                                 const std::string& initial_value = "",
                                 ReadCallback read_cb = nullptr,
                                 WriteCallback write_cb = nullptr);

    /**
     * @brief Handle GATT access operations (read/write).
     * @param conn_handle BLE connection handle
     * @param attr_handle GATT attribute handle
     * @param ctxt NimBLE access context
     * @return BLE error code (0 for success)
     */
    int handle_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt);

    /**
     * @brief Static callback for NimBLE GATT characteristic access.
     * @param conn_handle BLE connection handle
     * @param attr_handle GATT attribute handle
     * @param ctxt NimBLE access context
     * @param arg Pointer to CustomBLECharacteristic instance
     * @return BLE error code (0 for success)
     */
    static int gatt_access_callback(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt, void *arg);

    /**
     * @brief Get the UUID of this characteristic.
     * @return Pointer to the UUID structure
     */
    const ble_uuid_t* get_uuid() const;

    /**
     * @brief Get the GATT flags for this characteristic.
     * @return Flags (read/write/notify)
     */
    uint16_t get_flags() const;

    /**
     * @brief Set the GATT handle for this characteristic.
     * @param char_handle The handle assigned by NimBLE
     */
    void set_handle(uint16_t char_handle);

    /**
     * @brief Get the GATT handle for this characteristic.
     * @return The handle value
     */
    uint16_t get_handle() const;

    /**
     * @brief Set the read callback.
     * @param callback Function to call on read
     */
    void set_read_callback(ReadCallback callback);

    /**
     * @brief Set the write callback.
     * @param callback Function to call on write
     */
    void set_write_callback(WriteCallback callback);
};
