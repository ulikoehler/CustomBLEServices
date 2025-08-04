
# CustomBLEServices
Framework for simplifying adding custom BLE services/characteristics on the ESP32 (using NimBLE API)

## Simple Example: One Service, One Characteristic

```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>

using namespace CustomBLE;

// Create a 128-bit UUID for the service and characteristic
ble_uuid128_t service_uuid = {/* ... fill with your UUID ... */};
ble_uuid128_t char_uuid = {/* ... fill with your UUID ... */};

ServiceManager service_manager;
Service& service = service_manager.emplace_service(service_uuid);
service.emplace_characteristic(char_uuid, "Hello BLE!");

// Register with NimBLE (see ESP-IDF docs for details)
service_manager.add_services_to_nimble();
```

## Quick Start: Pointer-Based Characteristics

### Read-Only Characteristic from Pointer
```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>

int sensor_value = 42;
ble_uuid128_t char_uuid = {/* ... */};

ServiceManager service_manager;
Service& service = service_manager.emplace_service(service_uuid);

// Method 1: Using callback makers with emplace
auto read_cb = Characteristic::make_pointer_read_callback(&sensor_value);
service.emplace_characteristic(char_uuid, "", read_cb);

// Method 2: Using factory method to create complete characteristic
auto characteristic = Characteristic::from_pointer_read_only(char_uuid, &sensor_value);
service.add_characteristic(std::move(characteristic));
```

### Read-Write Characteristic from Pointer
```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>

float config_value = 3.14f;
ble_uuid128_t char_uuid = {/* ... */};

ServiceManager service_manager;
Service& service = service_manager.emplace_service(service_uuid);

// Method 1: Using callback makers with emplace
auto read_cb = Characteristic::make_pointer_read_callback(&config_value);
auto write_cb = Characteristic::make_pointer_write_callback(&config_value);
service.emplace_characteristic(char_uuid, "", read_cb, write_cb);

// Method 2: Using factory method to create complete characteristic
auto characteristic = Characteristic::from_pointer_read_write(char_uuid, &config_value);
service.add_characteristic(std::move(characteristic));
```

### Write-Only Characteristic from Pointer
```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>

int command_value = 0;
ble_uuid128_t char_uuid = {/* ... */};

ServiceManager service_manager;
Service& service = service_manager.emplace_service(service_uuid);

// Create write-only characteristic directly from pointer
auto characteristic = Characteristic::from_pointer_write_only(char_uuid, &command_value);
service.add_characteristic(std::move(characteristic));
```

## Comprehensive Usage Examples

### 1. Adding Characteristics Inline (Emplace)

```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>

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
#include <CustomBLE/CharacteristicsManager.hpp>

CharacteristicsManager char_manager;
char_manager.emplace_characteristic(char_uuid, "Value");
ble_gatt_chr_def* chr_defs = char_manager.get_chr_defs();
```

### 3. Full Service Setup and Registration

```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>

ServiceManager service_manager;
ble_uuid128_t service_uuid = {/* ... */};
Service& service = service_manager.emplace_service(service_uuid);

// Add multiple characteristics
service.emplace_characteristic(char_uuid1, "A");
service.emplace_characteristic(char_uuid2, "B");

// Register all services with NimBLE
service_manager.add_services_to_nimble();
```

### 4. Advanced: ServiceCharacteristicsManager


```cpp
#include <CustomBLE/ServiceCharacteristicsManager.hpp>

ServiceCharacteristicsManager adv_manager;
adv_manager.emplace_characteristic(char_uuid, "Advanced", read_cb, write_cb);
ble_gatt_chr_def* chr_defs = adv_manager.get_chr_defs();
```

### 5. Using GenericCallbacks for Read/Write

These callbacks simplify read/write operations when dealing with fixed or pointer-based values.

#### Pointer-Based Callbacks
```cpp
#include <CustomBLE/GenericCallbacks.hpp>
#include <CustomBLE/Characteristic.hpp>

int my_int = 42;
auto read_cb = make_pointer_read_callback(&my_int);
auto write_cb = make_pointer_write_callback(&my_int);

service.emplace_characteristic(char_uuid, "IntValue", read_cb, write_cb);
```

#### Fixed Value Callbacks
```cpp
#include <CustomBLE/GenericCallbacks.hpp>
#include <CustomBLE/Characteristic.hpp>

std::string my_value = "Initial";
auto read_cb = make_fixed_read_callback(my_value);
auto write_cb = make_fixed_write_callback(my_value);

service.emplace_characteristic(char_uuid, "FixedValue", read_cb, write_cb);
```

## Notes
    - All manager classes support emplace-like methods for inline construction.
    - Use the `CustomBLE` namespace for all types.
    - See ESP-IDF NimBLE documentation for registration details.

## Generating BLE UUID Macros

To easily generate a C++ macro for a 128-bit BLE UUID, use the provided script:

```sh
python3 generate_bluetooth_uuid.py
```

This will output a line starting with `BLE_UUID128_INIT(` containing the UUID bytes in the correct format for use in your code. No parameters are required.
