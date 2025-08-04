#include "CustomBLECharacteristicsManager.hpp"


namespace CustomBLE {

void CharacteristicsManager::add_characteristic(std::unique_ptr<Characteristic> characteristic) {
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

Characteristic& CharacteristicsManager::emplace_characteristic(const ble_uuid128_t& characteristic_uuid,
                                                               const std::string& initial_value,
                                                               Characteristic::ReadCallback read_cb,
                                                               Characteristic::WriteCallback write_cb) {
    auto characteristic = std::make_unique<Characteristic>(characteristic_uuid, initial_value, read_cb, write_cb);
    Characteristic* ptr = characteristic.get();
    add_characteristic(std::move(characteristic));
    return *ptr;
}

ble_gatt_chr_def* CustomBLECharacteristicsManager::get_chr_defs() {
    update_chr_defs();
    return chr_defs.data();
}

size_t CustomBLECharacteristicsManager::size() const {
    return entries.size();
}

void CustomBLECharacteristicsManager::update_chr_defs() {
    chr_defs.clear();
    for (const auto& entry : entries) {
        chr_defs.push_back(entry.chr_def);
    }
    // Always ensure the last element is the end marker
    ble_gatt_chr_def end_marker = {};
    end_marker.uuid = nullptr;
    chr_defs.push_back(end_marker);
}
