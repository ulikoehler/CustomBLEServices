#pragma once
#include "CustomBLEService.hpp"
#include <vector>
#include <memory>
#include <cstddef>

/**
 * @brief Manages multiple BLE GATT services and provides an array for NimBLE registration.
 */
class CustomBLEServiceManager {
private:
    std::vector<std::unique_ptr<CustomBLEService>> services;
    std::vector<ble_gatt_svc_def> svc_defs;

public:
    /**
     * @brief Add a new service to the manager.
     * @param service Unique pointer to a CustomBLEService
     */
    void add_service(std::unique_ptr<CustomBLEService> service);

    /**
     * @brief Get pointer to the array of ble_gatt_svc_def for NimBLE registration.
     * The last element is always the end marker.
     */
    ble_gatt_svc_def* get_svc_defs();

    /**
     * @brief Get the number of managed services (excluding end marker).
     */
    size_t size() const;

private:
    /**
     * @brief Update the internal svc_defs vector, ensuring the last element is the end marker.
     */
    void update_svc_defs();
};
