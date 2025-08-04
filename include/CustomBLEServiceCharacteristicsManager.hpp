#pragma once
#include "CustomBLECharacteristic.hpp"
#include <vector>
#include <memory>
#include <cstddef>

/**
 * @brief Manager for multiple BLE characteristics, providing a GATT characteristic array for service definition.
 */
class CustomBLECharacteristicsManager {
public:
    struct CharacteristicEntry {
        std::unique_ptr<CustomBLECharacteristic> characteristic;
        ble_gatt_chr_def chr_def;
    };

private:
    std::vector<CharacteristicEntry> entries;
    std::vector<ble_gatt_chr_def> chr_defs;

public:
    /**
     * @brief Add a new characteristic to the manager.
     * @param characteristic Unique pointer to a CustomBLECharacteristic
     */
    void add_characteristic(std::unique_ptr<CustomBLECharacteristic> characteristic);

    /**
     * @brief Get pointer to the array of ble_gatt_chr_def for service definition.
     * The last element is always the end marker.
     */
    ble_gatt_chr_def* get_chr_defs();

    /**
     * @brief Get the number of managed characteristics (excluding end marker).
     */
    size_t size() const;

private:
    /**
     * @brief Update the internal chr_defs vector, ensuring the last element is the end marker.
     */
    void update_chr_defs();
};
