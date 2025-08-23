#include "CustomBLE/ServiceManager.hpp"
#include <algorithm>
#include "esp_ble_conn_mgr.h"

namespace CustomBLE {
int ServiceManager::add_services_to_nimble(const char* tag) {
    ble_gatt_svc_def* svcs = get_svc_defs();
    int rc = ble_gatts_count_cfg(svcs);
    if (rc != 0) {
        ESP_LOGE(tag, "Failed to count GATT services: %d", rc);
        return rc;
    }
    rc = ble_gatts_add_svcs(svcs);
    if (rc != 0) {
        ESP_LOGE(tag, "Failed to add GATT services: %d", rc);
        return rc;
    }
    return 0;
}

std::shared_ptr<Service> ServiceManager::emplace_service(const ble_uuid128_t& uuid) {
    return emplace_service(nullptr, uuid);
}

std::shared_ptr<Service> ServiceManager::emplace_service(const char* name, const ble_uuid128_t& uuid) {
    auto service = std::make_shared<Service>(name, uuid);
    add_service(service);
    return service;
}

std::string ServiceManager::overview() const {
    std::string out = "ServiceManager overview:\n";
    size_t idx = 0;
    for (const auto& svc : services) {
        out += "Service [" + std::to_string(idx++) + "]:\n";
        out += svc->overview();
        out += "\n";
    }
    return out;
}

void ServiceManager::print() const {
    printf("%s", overview().c_str());
}

void ServiceManager::add_service(std::shared_ptr<Service> service) {
    services.push_back(std::move(service));
    update_svc_defs();
}

ble_gatt_svc_def* ServiceManager::get_svc_defs() {
    update_svc_defs();
    return svc_defs.data();
}

size_t ServiceManager::size() const {
    return services.size();
}

void ServiceManager::update_svc_defs() {
    svc_defs.clear();
    for (const auto& service : services) {
        ble_gatt_svc_def svc_def = service->get_svc_def();
        svc_defs.push_back(svc_def);
    }
    // Always ensure the last element is the end marker
    ble_gatt_svc_def end_marker = {};
    end_marker.type = 0;
    svc_defs.push_back(end_marker);
}

void ServiceManager::populate_adv_data(esp_ble_conn_config_t &config) {
    adv_data.clear();
    // For each registered service, if it has a 128-bit UUID, add a Complete List of 128-bit Service UUIDs AD element.
    // We will build a single AD element that contains all 128-bit UUIDs (if any).
    std::vector<const ble_uuid128_t*> uuids128;
    for (const auto &svc : services) {
        const ble_uuid128_t* u = svc->get_uuid();
        if (u) {
            uuids128.push_back(u);
        }
    }
    if (uuids128.empty()) {
        config.periodic_adv_data = nullptr;
        config.periodic_adv_len = 0;
        return;
    }

    // AD format: <len = 1 + (16 * n)><type=0x07><16*n bytes UUIDs (each little-endian)>
    size_t payload_len = 1 + uuids128.size() * 16; // 1 for type
    if (payload_len > 0xFF) {
        // unlikely, but guard
        payload_len = 0xFF;
    }
    adv_data.reserve(2 + payload_len);
    adv_data.push_back(static_cast<uint8_t>(payload_len));
    adv_data.push_back(0x07); // Complete list of 128-bit Service UUIDs

    for (const auto *u : uuids128) {
        // bleed_uuid128_t stores value[] likely little-endian already as used elsewhere
        for (int i = 0; i < 16; ++i) {
            adv_data.push_back(u->value[i]);
        }
    }

    // Populate both extended and periodic advertising fields so callers can
    // choose either mode at runtime (extended advertising or periodic adv).
    config.extended_adv_data = reinterpret_cast<const char*>(adv_data.data());
    config.extended_adv_len = adv_data.size();
    config.periodic_adv_data = reinterpret_cast<const char*>(adv_data.data());
    config.periodic_adv_len = adv_data.size();
}

} // namespace CustomBLE
