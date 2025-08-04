#include "CustomBLEService.hpp"

CustomBLEService::CustomBLEService(const ble_uuid128_t& uuid)
    : service_uuid(uuid) {
    svc_def = {};
    svc_def.type = BLE_GATT_SVC_TYPE_PRIMARY;
    svc_def.uuid = &service_uuid.u;
    svc_def.includes = nullptr;
    svc_def.chrs = nullptr; // Will be set in get_svc_defs()
    svc_defs.clear();
}

void CustomBLEService::add_characteristic(std::unique_ptr<CustomBLECharacteristic> characteristic) {
    characteristics_manager.add_characteristic(std::move(characteristic));
}

ble_gatt_svc_def* CustomBLEService::get_svc_defs() {
    svc_def.chrs = characteristics_manager.get_chr_defs();
    svc_defs.clear();
    svc_defs.push_back(svc_def);
    // End marker
    ble_gatt_svc_def end_marker = {};
    end_marker.type = 0;
    svc_defs.push_back(end_marker);
    return svc_defs.data();
}

CustomBLECharacteristicsManager& CustomBLEService::get_characteristics_manager() {
    return characteristics_manager;
}
