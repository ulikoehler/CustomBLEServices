
#pragma once
#include "CustomBLE/CharacteristicsManager.hpp"
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

public:
    Service(const ble_uuid128_t& uuid);
    void add_characteristic(std::shared_ptr<Characteristic> characteristic);
    void add_characteristic(Characteristic&& characteristic);
    ble_gatt_svc_def get_svc_def();
    CharacteristicsManager& get_characteristics_manager();

    /**
     * @brief Emplace a new characteristic inline (constructs and adds).
     * @param characteristic_uuid UUID of the characteristic
     * @param read_cb Optional read callback
     * @param write_cb Optional write callback
     * @return Shared pointer to the newly added Characteristic
     */
    std::shared_ptr<Characteristic> emplace_characteristic(const ble_uuid128_t& characteristic_uuid,
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
    /**
     * @brief Return pointer to the internal 128-bit UUID value
     */
    const ble_uuid128_t* get_uuid() const { return &service_uuid; }
};

} // namespace CustomBLE
