

#pragma once
#include "CustomBLE/Service.hpp"
#include <vector>
#include <memory>
#include <cstddef>
#include <deque>
#include <esp_ble_conn_mgr.h>

namespace CustomBLE {

class ServiceManager {
private:
    std::vector<std::shared_ptr<Service>> services;
    std::vector<ble_gatt_svc_def> svc_defs;
    std::vector<uint8_t> adv_data; // persistent buffer used when populating advertising data
    std::vector<std::vector<esp_ble_conn_character_t>> conn_mgr_characteristics;
    std::vector<esp_ble_conn_svc_t> conn_mgr_services;
    std::deque<std::string> generated_characteristic_names;

public:
    void add_service(std::shared_ptr<Service> service);

    /**
     * @brief Emplace a new service inline (constructs and adds).
     * @param uuid UUID of the service
     * @return Shared pointer to the newly added Service
     */
    std::shared_ptr<Service> emplace_service(const ble_uuid128_t& uuid) __attribute__((deprecated("Use emplace_service(const char*, const ble_uuid128_t&)")));
    std::shared_ptr<Service> emplace_service(const char* name, const ble_uuid128_t& uuid);
    ble_gatt_svc_def* get_svc_defs();
    size_t size() const;

    /**
     * @brief Add all services to NimBLE using ble_gatts_count_cfg and ble_gatts_add_svcs.
     *
     * Ensures that NimBLE FreeRTOS NPL (npl_funcs + mutex pools) is initialized:
     * esp_ble_conn_init() may differ from the "classic" nimble_port_init(); without NPL
     * ble_gatts_add_svcs() fails inside ble_hs_lock()/ble_npl_mutex_pend (e.g. EXCVADDR 0x44).
     *
     * @param tag Logging tag for ESP_LOGE
     * @return 0 on success, error code otherwise
     */
    int add_services_to_nimble(const char* tag = "CustomBLE");
    esp_err_t register_with_conn_mgr();
    
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
    static esp_err_t ble_conn_access_cb(const uint8_t *inbuf,
                                        uint16_t inlen,
                                        uint8_t **outbuf,
                                        uint16_t *outlen,
                                        void *priv_data,
                                        uint8_t *att_status);
    void update_svc_defs();
};

} // namespace CustomBLE
