

#pragma once
#include "CustomBLE/Service.hpp"
#include <vector>
#include <memory>
#include <cstddef>
#include <esp_ble_conn_mgr.h>

namespace CustomBLE {

class ServiceManager {
private:
    std::vector<std::shared_ptr<Service>> services;
    std::vector<ble_gatt_svc_def> svc_defs;
    std::vector<uint8_t> adv_data; // persistent buffer used when populating advertising data

public:
    void add_service(std::shared_ptr<Service> service);

    /**
     * @brief Emplace a new service inline (constructs and adds).
     * @param uuid UUID of the service
     * @return Shared pointer to the newly added Service
     */
    std::shared_ptr<Service> emplace_service(const ble_uuid128_t& uuid);
    ble_gatt_svc_def* get_svc_defs();
    size_t size() const;

    /**
     * @brief Add all services to NimBLE using ble_gatts_count_cfg and ble_gatts_add_svcs.
     * @param tag Logging tag for ESP_LOGE
     * @return 0 on success, error code otherwise
     */
    int add_services_to_nimble(const char* tag = "CustomBLE");
    
    /**
     * @brief Generate a string overview of all services.
     */
    std::string overview() const;

    /**
     * @brief Print the overview using printf().
     */
    void print() const;
public:
    /**
     * @brief Populate the provided esp_ble_conn_config_t with advertisement bytes
     * that announce registered services (supports 128-bit UUIDs).
     * This fills the internal adv_data buffer so the pointer remains valid
     * after the call.
     */
    void populate_adv_data(esp_ble_conn_config_t &config);
private:
    void update_svc_defs();
};

} // namespace CustomBLE
