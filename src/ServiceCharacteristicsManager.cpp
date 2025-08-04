Characteristic& ServiceCharacteristicsManager::emplace_characteristic(const ble_uuid128_t& characteristic_uuid,
                                                                     const std::string& initial_value,
                                                                     Characteristic::ReadCallback read_cb,
                                                                     Characteristic::WriteCallback write_cb) {
    auto characteristic = std::make_unique<Characteristic>(characteristic_uuid, initial_value, read_cb, write_cb);
    Characteristic* ptr = characteristic.get();
    add_characteristic(std::move(characteristic));
    return *ptr;
}

#include "CustomBLEServiceCharacteristicsManager.hpp"

namespace CustomBLE {
std::string ServiceCharacteristicsManager::overview() const {
    std::string out = "ServiceCharacteristicsManager overview:\n";
    size_t idx = 0;
    for (const auto& entry : entries) {
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

void ServiceCharacteristicsManager::print() const {
    printf("%s", overview().c_str());
}

void ServiceCharacteristicsManager::add_characteristic(std::unique_ptr<Characteristic> characteristic) {
    CharacteristicEntry entry;
    entry.characteristic = std::move(characteristic);
    entry.chr_def = {
        entry.characteristic->get_uuid(),
        Characteristic::gatt_access_callback,
        entry.characteristic.get(),
        nullptr, // descriptors
        entry.characteristic->get_flags(),
        0, // min_key_size
        &entry.characteristic->handle,
        nullptr // cpfd
    };
    entries.push_back(std::move(entry));
    update_chr_defs();
}

ble_gatt_chr_def* ServiceCharacteristicsManager::get_chr_defs() {
    update_chr_defs();
    return chr_defs.data();
}

size_t ServiceCharacteristicsManager::size() const {
    return entries.size();
}

void ServiceCharacteristicsManager::update_chr_defs() {
    chr_defs.clear();
    for (const auto& entry : entries) {
        chr_defs.push_back(entry.chr_def);
    }
    // Always ensure the last element is the end marker
    ble_gatt_chr_def end_marker = {};
    end_marker.uuid = nullptr;
    chr_defs.push_back(end_marker);
}

} // namespace CustomBLE
