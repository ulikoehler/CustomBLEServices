
#include "CustomBLE/CharacteristicsManager.hpp"

namespace CustomBLE {
std::string CharacteristicsManager::overview() const {
    std::string out = "Characteristics:\n";
    size_t idx = 0;
    for (const auto& entry : entries) {
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

void CharacteristicsManager::print() const {
    printf("%s", overview().c_str());
}

void CharacteristicsManager::add_characteristic(std::shared_ptr<Characteristic> characteristic) {
    CharacteristicEntry entry;
    entry.characteristic = std::move(characteristic);
    entry.chr_def = {
        entry.characteristic->get_uuid(),
        Characteristic::gatt_access_callback,
        entry.characteristic.get(),
        nullptr, // descriptors
        entry.characteristic->get_flags(),
        0, // min_key_size
        nullptr, // handle pointer (set elsewhere if needed)
        nullptr // cpfd
    };
    entries.push_back(std::move(entry));
    update_chr_defs();
}

std::shared_ptr<Characteristic> CharacteristicsManager::emplace_characteristic(const ble_uuid128_t& characteristic_uuid,
                                                               const std::string& initial_value,
                                                               Characteristic::ReadCallback read_cb,
                                                               Characteristic::WriteCallback write_cb) {
    auto characteristic = std::make_shared<Characteristic>(characteristic_uuid, initial_value, read_cb, write_cb);
    add_characteristic(characteristic);
    return characteristic;
}

ble_gatt_chr_def* CharacteristicsManager::get_chr_defs() {
    update_chr_defs();
    return chr_defs.data();
}

size_t CharacteristicsManager::size() const {
    return entries.size();
}

void CharacteristicsManager::update_chr_defs() {
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
