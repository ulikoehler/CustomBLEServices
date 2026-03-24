#include "CustomBLE/ServiceManager.hpp"
#include <algorithm>
#include <cstdlib>
#include <unordered_map>
#include "esp_ble_conn_mgr.h"

namespace CustomBLE {
namespace {

std::unordered_map<std::string, Characteristic*> g_ble_conn_characteristics;

uint16_t convert_flags(uint16_t flags) {
    uint16_t converted = 0;

    if (flags & BLE_GATT_CHR_F_READ) {
        converted |= BLE_CONN_GATT_CHR_READ;
    }
    if (flags & BLE_GATT_CHR_F_WRITE) {
        converted |= BLE_CONN_GATT_CHR_WRITE;
    }
    if (flags & BLE_GATT_CHR_F_WRITE_NO_RSP) {
        converted |= BLE_CONN_GATT_CHR_WRITE_NO_RSP;
    }
    if (flags & BLE_GATT_CHR_F_NOTIFY) {
        converted |= BLE_CONN_GATT_CHR_NOTIFY;
    }
    if (flags & BLE_GATT_CHR_F_INDICATE) {
        converted |= BLE_CONN_GATT_CHR_INDICATE;
    }

    return converted;
}

} // namespace

int ServiceManager::add_services_to_nimble(const char* tag) {
    ble_gatt_svc_def* svcs = get_svc_defs();
    if (svcs == nullptr) {
        ESP_LOGE(tag, "Service definition pointer is null (services=%u, svc_defs=%u)",
                 static_cast<unsigned>(services.size()),
                 static_cast<unsigned>(svc_defs.size()));
        return BLE_HS_EINVAL;
    }

    // Debug: dump svc_defs to verify pointer validity before calling NimBLE
    for (int s = 0; svcs[s].type != BLE_GATT_SVC_TYPE_END; s++) {
        ESP_LOGI(tag, "svc[%d]: type=%u uuid=%p chr=%p", s,
                 (unsigned)svcs[s].type, (void*)svcs[s].uuid, (void*)svcs[s].characteristics);
        if (svcs[s].characteristics) {
            for (int c = 0; svcs[s].characteristics[c].uuid != nullptr; c++) {
                ESP_LOGI(tag, "  chr[%d]: uuid=%p access_cb=%p dsc=%p flags=0x%x", c,
                         (void*)svcs[s].characteristics[c].uuid,
                         (void*)svcs[s].characteristics[c].access_cb,
                         (void*)svcs[s].characteristics[c].descriptors,
                         (unsigned)svcs[s].characteristics[c].flags);
            }
        }
    }

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

esp_err_t ServiceManager::ble_conn_access_cb(const uint8_t *inbuf,
                                             uint16_t inlen,
                                             uint8_t **outbuf,
                                             uint16_t *outlen,
                                             void *priv_data,
                                             uint8_t *att_status) {
    if (outbuf) {
        *outbuf = nullptr;
    }
    if (outlen) {
        *outlen = 0;
    }
    if (att_status) {
        *att_status = ESP_IOT_ATT_SUCCESS;
    }

    const char* characteristic_name = static_cast<const char*>(priv_data);
    if (!characteristic_name) {
        if (att_status) {
            *att_status = ESP_IOT_ATT_INVALID_HANDLE;
        }
        return ESP_ERR_INVALID_ARG;
    }

    auto it = g_ble_conn_characteristics.find(characteristic_name);
    if (it == g_ble_conn_characteristics.end() || !it->second) {
        if (att_status) {
            *att_status = ESP_IOT_ATT_INVALID_HANDLE;
        }
        return ESP_ERR_NOT_FOUND;
    }

    Characteristic* characteristic = it->second;
    if (!inbuf) {
        std::string value = characteristic->read_value();
        if (outlen) {
            *outlen = static_cast<uint16_t>(value.size());
        }
        if (!value.empty() && outbuf) {
            *outbuf = static_cast<uint8_t*>(malloc(value.size()));
            if (!*outbuf) {
                if (att_status) {
                    *att_status = ESP_IOT_ATT_INSUF_RESOURCE;
                }
                if (outlen) {
                    *outlen = 0;
                }
                return ESP_ERR_NO_MEM;
            }
            memcpy(*outbuf, value.data(), value.size());
        }
        return ESP_OK;
    }

    characteristic->write_value(std::string(reinterpret_cast<const char*>(inbuf), inlen));
    return ESP_OK;
}

esp_err_t ServiceManager::register_with_conn_mgr() {
    conn_mgr_characteristics.clear();
    conn_mgr_services.clear();
    generated_characteristic_names.clear();
    g_ble_conn_characteristics.clear();

    conn_mgr_characteristics.reserve(services.size());
    conn_mgr_services.reserve(services.size());

    for (const auto& service : services) {
        if (!service) {
            continue;
        }

        const auto& entries = service->get_characteristics_manager().get_entries();
        if (entries.empty()) {
            continue;
        }

        conn_mgr_characteristics.emplace_back();
        auto& chars = conn_mgr_characteristics.back();
        chars.reserve(entries.size());

        for (size_t index = 0; index < entries.size(); ++index) {
            const auto& entry = entries[index];
            const char* name = entry.characteristic->get_name();
            if (!name || !*name) {
                const ble_uuid128_t* uuid = reinterpret_cast<const ble_uuid128_t*>(entry.characteristic->get_uuid());
                char generated_name[48];
                snprintf(generated_name, sizeof(generated_name),
                         "customble-%02x%02x%02x%02x-%zu",
                         uuid->value[0], uuid->value[1], uuid->value[2], uuid->value[3], index);
                generated_characteristic_names.emplace_back(generated_name);
                name = generated_characteristic_names.back().c_str();
            }

            esp_ble_conn_character_t chr = {};
            chr.name = name;
            chr.type = BLE_CONN_UUID_TYPE_128;
            memcpy(chr.uuid.uuid128,
                   reinterpret_cast<const ble_uuid128_t*>(entry.characteristic->get_uuid())->value,
                   BLE_UUID128_VAL_LEN);
            chr.flag = convert_flags(entry.characteristic->get_flags());
            chr.uuid_fn = &ServiceManager::ble_conn_access_cb;
            chars.push_back(chr);
            g_ble_conn_characteristics[name] = entry.characteristic.get();
        }

        esp_ble_conn_svc_t svc = {};
        svc.type = BLE_CONN_UUID_TYPE_128;
        svc.nu_lookup_count = static_cast<uint16_t>(chars.size());
        memcpy(svc.uuid.uuid128, service->get_uuid()->value, BLE_UUID128_VAL_LEN);
        svc.nu_lookup = chars.data();
        conn_mgr_services.push_back(svc);
    }

    for (const auto& svc : conn_mgr_services) {
        esp_err_t err = esp_ble_conn_add_svc(&svc);
        if (err != ESP_OK) {
            return err;
        }
    }

    return ESP_OK;
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
    if (svc_defs.empty()) {
        return nullptr;
    }
    return svc_defs.data();
}

size_t ServiceManager::size() const {
    return services.size();
}

void ServiceManager::update_svc_defs() {
    svc_defs.clear();
    svc_defs.reserve(services.size() + 1);

    for (const auto& service : services) {
        if (!service) {
            continue;
        }
        ble_gatt_svc_def svc_def = service->get_svc_def();
        if (svc_def.type == BLE_GATT_SVC_TYPE_END) {
            continue;
        }
        if (svc_def.uuid == nullptr) {
            continue;
        }
        svc_defs.push_back(svc_def);
    }

    // Always ensure the last element is the end marker
    ble_gatt_svc_def end_marker = {};
    end_marker.type = BLE_GATT_SVC_TYPE_END;
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
