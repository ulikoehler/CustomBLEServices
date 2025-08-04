
#pragma once
#include "CustomBLECharacteristicsManager.hpp"
#include "host/ble_uuid.h"
#include "host/ble_gatt.h"
#include <memory>
#include <vector>

namespace CustomBLE {

class Service {
private:
    ble_uuid128_t service_uuid;
    CharacteristicsManager characteristics_manager;
    ble_gatt_svc_def svc_def;
    std::vector<ble_gatt_svc_def> svc_defs;

public:
    Service(const ble_uuid128_t& uuid);
    void add_characteristic(std::unique_ptr<Characteristic> characteristic);
    ble_gatt_svc_def* get_svc_defs();
    CharacteristicsManager& get_characteristics_manager();

    /**
     * @brief Emplace a new characteristic inline (constructs and adds).
     * @param characteristic_uuid UUID of the characteristic
     * @param initial_value Initial value
     * @param read_cb Optional read callback
     * @param write_cb Optional write callback
     * @return Reference to the newly added Characteristic
     */
    Characteristic& emplace_characteristic(const ble_uuid128_t& characteristic_uuid,
                                           const std::string& initial_value,
                                           Characteristic::ReadCallback read_cb = nullptr,
                                           Characteristic::WriteCallback write_cb = nullptr);

    /**
     * @brief Generate a string overview of the service and its characteristics.
     */
    std::string overview() const;

    /**
     * @brief Print the overview using printf().
     */
    void print() const;
};

} // namespace CustomBLE
