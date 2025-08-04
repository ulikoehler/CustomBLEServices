
#include "CustomBLEService.hpp"

namespace CustomBLE {

Service::Service(const ble_uuid128_t& uuid)
    : service_uuid(uuid) {
    svc_def = {};
    svc_def.type = BLE_GATT_SVC_TYPE_PRIMARY;
    svc_def.uuid = &service_uuid.u;
    svc_def.includes = nullptr;
    svc_def.chrs = nullptr; // Will be set in get_svc_defs()
    svc_defs.clear();
}

void Service::add_characteristic(std::unique_ptr<Characteristic> characteristic) {
    characteristics_manager.add_characteristic(std::move(characteristic));
}

ble_gatt_svc_def* Service::get_svc_defs() {
    svc_def.chrs = characteristics_manager.get_chr_defs();
    svc_defs.clear();
    svc_defs.push_back(svc_def);
    // End marker
    ble_gatt_svc_def end_marker = {};
    end_marker.type = 0;
    svc_defs.push_back(end_marker);
    return svc_defs.data();
}

CharacteristicsManager& Service::get_characteristics_manager() {
    return characteristics_manager;
}

std::string Service::overview() const {
    char uuid_str[40];
    snprintf(uuid_str, sizeof(uuid_str),
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        service_uuid.u.u128[0], service_uuid.u.u128[1], service_uuid.u.u128[2], service_uuid.u.u128[3],
        service_uuid.u.u128[4], service_uuid.u.u128[5],
        service_uuid.u.u128[6], service_uuid.u.u128[7],
        service_uuid.u.u128[8], service_uuid.u.u128[9],
        service_uuid.u.u128[10], service_uuid.u.u128[11], service_uuid.u.u128[12], service_uuid.u.u128[13], service_uuid.u.u128[14], service_uuid.u.u128[15]);
    std::string out = "Service UUID: ";
    out += uuid_str;
    out += "\nCharacteristics:\n";
    size_t idx = 0;
    for (const auto& entry : characteristics_manager.entries) {
        char char_uuid_str[40];
        const ble_uuid128_t* cu = reinterpret_cast<const ble_uuid128_t*>(entry.characteristic->get_uuid());
        snprintf(char_uuid_str, sizeof(char_uuid_str),
            "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            cu->u.u128[0], cu->u.u128[1], cu->u.u128[2], cu->u.u128[3],
            cu->u.u128[4], cu->u.u128[5],
            cu->u.u128[6], cu->u.u128[7],
            cu->u.u128[8], cu->u.u128[9],
            cu->u.u128[10], cu->u.u128[11], cu->u.u128[12], cu->u.u128[13], cu->u.u128[14], cu->u.u128[15]);
        out += "  [" + std::to_string(idx++) + "] UUID: ";
        out += char_uuid_str;
        out += "\n";
    }
    return out;
}

void Service::print() const {
    printf("%s", overview().c_str());
}

Characteristic& Service::emplace_characteristic(const ble_uuid128_t& characteristic_uuid,
                                                const std::string& initial_value,
                                                Characteristic::ReadCallback read_cb,
                                                Characteristic::WriteCallback write_cb) {
    return characteristics_manager.emplace_characteristic(characteristic_uuid, initial_value, read_cb, write_cb);
}

} // namespace CustomBLE
