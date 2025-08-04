Service& ServiceManager::emplace_service(const ble_uuid128_t& uuid) {
    auto service = std::make_unique<Service>(uuid);
    Service* ptr = service.get();
    add_service(std::move(service));
    return *ptr;
}

#include "CustomBLEServiceManager.hpp"

namespace CustomBLE {

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
