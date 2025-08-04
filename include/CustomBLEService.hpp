#pragma once
#include "CustomBLECharacteristicsManager.hpp"
#include "host/ble_uuid.h"
#include "host/ble_gatt.h"
#include <memory>

/**
 * @brief Manages a single BLE GATT service and its characteristics.
 */
class CustomBLEService {
private:
    ble_uuid128_t service_uuid;
    CustomBLECharacteristicsManager characteristics_manager;
    ble_gatt_svc_def svc_def;
    std::vector<ble_gatt_svc_def> svc_defs;

public:
    /**
     * @brief Construct a new CustomBLEService object.
     * @param uuid 128-bit UUID for the service
     */
    CustomBLEService(const ble_uuid128_t& uuid);

    /**
     * @brief Add a characteristic to the service.
     * @param characteristic Unique pointer to a CustomBLECharacteristic
     */
    void add_characteristic(std::unique_ptr<CustomBLECharacteristic> characteristic);

    /**
     * @brief Get pointer to the array of ble_gatt_svc_def for NimBLE registration.
     * The last element is always the end marker.
     */
    ble_gatt_svc_def* get_svc_defs();

    /**
     * @brief Get the underlying characteristics manager.
     */
    CustomBLECharacteristicsManager& get_characteristics_manager();
};
