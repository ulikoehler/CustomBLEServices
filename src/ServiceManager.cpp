Service& ServiceManager::emplace_service(const ble_uuid128_t& uuid) {
    auto service = std::make_unique<Service>(uuid);
    Service* ptr = service.get();
    add_service(std::move(service));
    return *ptr;
}

#include "CustomBLEServiceManager.hpp"

namespace CustomBLE {
int ServiceManager::add_services_to_nimble(const char* tag) const {
    ble_gatt_svc_def* svcs = const_cast<ble_gatt_svc_def*>(get_svc_defs());
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

void ServiceManager::add_service(std::unique_ptr<Service> service) {
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
        ble_gatt_svc_def* svc_array = service->get_svc_defs();
        // Add all but the end marker from each service
        for (size_t i = 0; svc_array[i].type != 0; ++i) {
            svc_defs.push_back(svc_array[i]);
        }
    }
    // Always ensure the last element is the end marker
    ble_gatt_svc_def end_marker = {};
    end_marker.type = 0;
    svc_defs.push_back(end_marker);
}

} // namespace CustomBLE
