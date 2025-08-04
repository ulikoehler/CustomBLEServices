#include "CustomBLE/Characteristic.hpp"

static const char *TAG = "CustomBLE/Characteristic";

namespace CustomBLE {
std::string Characteristic::overview() const {
    char uuid_str[40];
    // BLE UUID 128-bit: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    // The first three fields are little-endian, the rest are big-endian
    snprintf(uuid_str, sizeof(uuid_str),
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        // time_low (4 bytes, little-endian)
        uuid.value[3], uuid.value[2], uuid.value[1], uuid.value[0],
        // time_mid (2 bytes, little-endian)
        uuid.value[5], uuid.value[4],
        // time_hi_and_version (2 bytes, little-endian)
        uuid.value[7], uuid.value[6],
        // clock_seq_hi_res, clock_seq_low (2 bytes, big-endian)
        uuid.value[8], uuid.value[9],
        // node (6 bytes, big-endian)
        uuid.value[10], uuid.value[11], uuid.value[12], uuid.value[13], uuid.value[14], uuid.value[15]);
    std::string out = "Characteristic UUID: ";
    out += uuid_str;
    out += "\n";
    return out;
}

void Characteristic::print() const {
    printf("%s", overview().c_str());
}

Characteristic::Characteristic(const ble_uuid128_t& characteristic_uuid,
                                 ReadCallback read_cb,
                                 WriteCallback write_cb)
    : uuid(characteristic_uuid), handle(0), read_callback(read_cb),
      write_callback(write_cb) {
    // Set flags based on available callbacks
    flags = 0;
    if (read_callback) {
        flags |= BLE_GATT_CHR_F_READ;
    }
    if (write_callback) {
        flags |= BLE_GATT_CHR_F_WRITE;
    }
    // Always allow notify if we have read capability
    if (flags & BLE_GATT_CHR_F_READ) {
        flags |= BLE_GATT_CHR_F_NOTIFY;
    }
}

int Characteristic::handle_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt) {
    int rc;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR: {
            ESP_LOGI(TAG, "Characteristic read (handle: %d)", attr_handle);
            std::string value;
            if (read_callback) {
                value = read_callback();
            } else {
                value = "";
            }
            rc = os_mbuf_append(ctxt->om, value.c_str(), value.length());
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        case BLE_GATT_ACCESS_OP_WRITE_CHR: {
            ESP_LOGI(TAG, "Characteristic write (handle: %d)", attr_handle);
            uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);
            if (om_len > 0) {
                char buffer[om_len + 1];
                rc = ble_hs_mbuf_to_flat(ctxt->om, buffer, sizeof(buffer) - 1, NULL);
                if (rc == 0) {
                    buffer[om_len] = '\0';
                    std::string received_value(buffer);
                    if (write_callback) {
                        write_callback(received_value);
                    }
                    ESP_LOGI(TAG, "Characteristic updated to: %s", received_value.c_str());
                }
            }
            return 0;
        }
        default:
            return BLE_ATT_ERR_UNLIKELY;
    }
}

int Characteristic::gatt_access_callback(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt, void *arg) {
    Characteristic* characteristic = static_cast<Characteristic*>(arg);
    return characteristic->handle_access(conn_handle, attr_handle, ctxt);
}

const ble_uuid_t* Characteristic::get_uuid() const {
    return &uuid.u;
}

uint16_t Characteristic::get_flags() const {
    return flags;
}

void Characteristic::set_handle(uint16_t char_handle) {
    handle = char_handle;
}

uint16_t Characteristic::get_handle() const {
    return handle;
}

void Characteristic::set_read_callback(ReadCallback callback) {
    read_callback = callback;
    if (callback && !(flags & BLE_GATT_CHR_F_READ)) {
        flags |= BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY;
    }
}

void Characteristic::set_write_callback(WriteCallback callback) {
    write_callback = callback;
    if (callback && !(flags & BLE_GATT_CHR_F_WRITE)) {
        flags |= BLE_GATT_CHR_F_WRITE;
    }

} // namespace CustomBLE
}
