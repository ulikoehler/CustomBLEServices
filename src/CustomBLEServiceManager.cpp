#include "CustomBLEServiceManager.hpp"

void CustomBLEServiceManager::add_service(std::unique_ptr<CustomBLEService> service) {
    services.push_back(std::move(service));
    update_svc_defs();
}

ble_gatt_svc_def* CustomBLEServiceManager::get_svc_defs() {
    update_svc_defs();
    return svc_defs.data();
}

size_t CustomBLEServiceManager::size() const {
    return services.size();
}

void CustomBLEServiceManager::update_svc_defs() {
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
