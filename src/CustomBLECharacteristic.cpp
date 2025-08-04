#include "CustomBLECharacteristic.hpp"

static const char *TAG = "MetexonBLE";

CustomBLECharacteristic::CustomBLECharacteristic(const ble_uuid128_t& characteristic_uuid, 
                                 const std::string& initial_value,
                                 ReadCallback read_cb,
                                 WriteCallback write_cb)
    : uuid(characteristic_uuid), handle(0), read_callback(read_cb), 
      write_callback(write_cb), default_value(initial_value) {
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

int CustomBLECharacteristic::handle_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt) {
    int rc;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR: {
            ESP_LOGI(TAG, "Characteristic read (handle: %d)", attr_handle);
            std::string value;
            if (read_callback) {
                value = read_callback();
            } else {
                value = default_value;
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
                    } else {
                        default_value = received_value;
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

int CustomBLECharacteristic::gatt_access_callback(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt, void *arg) {
    CustomBLECharacteristic* characteristic = static_cast<CustomBLECharacteristic*>(arg);
    return characteristic->handle_access(conn_handle, attr_handle, ctxt);
}

const ble_uuid_t* CustomBLECharacteristic::get_uuid() const {
    return &uuid.u;
}

uint16_t CustomBLECharacteristic::get_flags() const {
    return flags;
}

void CustomBLECharacteristic::set_handle(uint16_t char_handle) {
    handle = char_handle;
}

uint16_t CustomBLECharacteristic::get_handle() const {
    return handle;
}

void CustomBLECharacteristic::set_read_callback(ReadCallback callback) {
    read_callback = callback;
    if (callback && !(flags & BLE_GATT_CHR_F_READ)) {
        flags |= BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY;
    }
}

void CustomBLECharacteristic::set_write_callback(WriteCallback callback) {
    write_callback = callback;
    if (callback && !(flags & BLE_GATT_CHR_F_WRITE)) {
        flags |= BLE_GATT_CHR_F_WRITE;
    }
}
