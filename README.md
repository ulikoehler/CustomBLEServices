
# CustomBLEServices
Framework for simplifying adding custom BLE services/characteristics on the ESP32 (using NimBLE API)

## Simple Example: One Service, One Characteristic

```cpp
#include <CustomBLEServiceManager.hpp>
#include <CustomBLEService.hpp>
#include <CustomBLECharacteristic.hpp>

using namespace CustomBLE;

// Create a 128-bit UUID for the service and characteristic
ble_uuid128_t service_uuid = {/* ... fill with your UUID ... */};
ble_uuid128_t char_uuid = {/* ... fill with your UUID ... */};

ServiceManager service_manager;
Service& service = service_manager.emplace_service(service_uuid);
service.emplace_characteristic(char_uuid, "Hello BLE!");

// Register with NimBLE (see ESP-IDF docs for details)
ble_gatt_svc_def* svc_defs = service_manager.get_svc_defs();
// ...register svc_defs with NimBLE...
```

## Comprehensive Usage Examples

### 1. Adding Characteristics Inline (Emplace)

```cpp
ServiceManager service_manager;
ble_uuid128_t service_uuid = {/* ... */};
Service& service = service_manager.emplace_service(service_uuid);

// Add a characteristic with custom read/write callbacks
ble_uuid128_t char_uuid = {/* ... */};
std::string my_value = "Initial";
auto read_cb = [&]() { return my_value; };
auto write_cb = [&](const std::string& v) { my_value = v; };
service.emplace_characteristic(char_uuid, my_value, read_cb, write_cb);
```

### 2. Using Manager Classes Directly

```cpp
CharacteristicsManager char_manager;
char_manager.emplace_characteristic(char_uuid, "Value");
ble_gatt_chr_def* chr_defs = char_manager.get_chr_defs();
```

### 3. Full Service Setup and Registration

```cpp
ServiceManager service_manager;
ble_uuid128_t service_uuid = {/* ... */};
Service& service = service_manager.emplace_service(service_uuid);

// Add multiple characteristics
service.emplace_characteristic(char_uuid1, "A");
service.emplace_characteristic(char_uuid2, "B");

// Register all services with NimBLE
ble_gatt_svc_def* svc_defs = service_manager.get_svc_defs();
// ...register svc_defs with NimBLE...
```

### 4. Advanced: ServiceCharacteristicsManager

```cpp
ServiceCharacteristicsManager adv_manager;
adv_manager.emplace_characteristic(char_uuid, "Advanced", read_cb, write_cb);
ble_gatt_chr_def* chr_defs = adv_manager.get_chr_defs();
```

## Notes
- All manager classes support emplace-like methods for inline construction.
- Use the `CustomBLE` namespace for all types.
- See ESP-IDF NimBLE documentation for registration details.
