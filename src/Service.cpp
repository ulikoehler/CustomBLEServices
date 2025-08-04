#include "CustomBLE/Service.hpp"

namespace CustomBLE {

Service::Service(const ble_uuid128_t& uuid)
    : service_uuid(uuid) {
    svc_def = {};
    svc_def.type = BLE_GATT_SVC_TYPE_PRIMARY;
    svc_def.uuid = &service_uuid.u;
    svc_def.includes = nullptr;
    // svc_def.chrs = nullptr; // Will be set in get_svc_def() -- REMOVE, not present in ble_gatt_svc_def
}

void Service::add_characteristic(std::shared_ptr<Characteristic> characteristic) {
    characteristics_manager.add_characteristic(std::move(characteristic));
}

void Service::add_characteristic(Characteristic&& characteristic) {
    characteristics_manager.add_characteristic(std::make_unique<Characteristic>(std::move(characteristic)));
}

ble_gatt_svc_def Service::get_svc_def() {
    svc_def.characteristics = characteristics_manager.get_chr_defs();
    return svc_def;
}

CharacteristicsManager& Service::get_characteristics_manager() {
    return characteristics_manager;
}

std::string Service::overview() const {
    char uuid_str[40];
    snprintf(uuid_str, sizeof(uuid_str),
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        service_uuid.value[0], service_uuid.value[1], service_uuid.value[2], service_uuid.value[3],
        service_uuid.value[4], service_uuid.value[5],
        service_uuid.value[6], service_uuid.value[7],
        service_uuid.value[8], service_uuid.value[9],
        service_uuid.value[10], service_uuid.value[11], service_uuid.value[12], service_uuid.value[13], service_uuid.value[14], service_uuid.value[15]);
    std::string out = "Service UUID: ";
    out += uuid_str;
    out += "\nCharacteristics:\n";
    size_t idx = 0;
    for (const auto& entry : characteristics_manager.get_entries()) {
        char char_uuid_str[40];
        const ble_uuid128_t* cu = reinterpret_cast<const ble_uuid128_t*>(entry.characteristic->get_uuid());
        snprintf(char_uuid_str, sizeof(char_uuid_str),
            "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            cu->value[0], cu->value[1], cu->value[2], cu->value[3],
            cu->value[4], cu->value[5],
            cu->value[6], cu->value[7],
            cu->value[8], cu->value[9],
            cu->value[10], cu->value[11], cu->value[12], cu->value[13], cu->value[14], cu->value[15]);
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
