
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
std::shared_ptr<Service> service = service_manager.emplace_service(service_uuid);
service->add_characteristic(Characteristic::from_fixed_value(char_uuid, "Hello BLE!"));

// Register with NimBLE (see ESP-IDF docs for details)
service_manager.add_services_to_nimble();
```

For example, here's how to add two characteristics with fixed values `A` and `B`:

```cpp
service->add_characteristic(Characteristic::from_fixed_value(
    BLE_UUID128_INIT(0xD7, 0xE0, 0x7D, 0xC9, 0x8B, 0xC1, 0x6D, 0x4E, 0xB9, 0xCC, 0x87, 0x82, 0xD8, 0xAA, 0x42, 0xC5),
    "A")
);
service->add_characteristic(Characteristic::from_fixed_value(
    BLE_UUID128_INIT(0x38, 0x7B, 0x0B, 0x64, 0xC8, 0x46, 0xC6, 0x46, 0xB9, 0xC9, 0x76, 0x4C, 0x87, 0xFC, 0x75, 0xA6),
    "B")
);
```

## Binary Conversion Utility: ToBinaryString

The `ToBinaryString` template function allows you to convert any C++ datatype (such as `int`, `float`, or structs) into a `std::string` (which *CustomBLE* uses for memory management) containing its raw binary representation..

**Header:**
```cpp
#include <CustomBLE/DataConversion.hpp>
```

**Example Usage:**
```cpp
#include <CustomBLE/DataConversion.hpp>
#include <iostream>

struct MyStruct {
    int a;
    float b;
};

int main() {
    int x = 42;
    float y = 3.14f;
    MyStruct s{7, 2.71f};

    std::string bin_x = ToBinaryString(x);      // bin_x.size() == sizeof(int)
    std::string bin_y = ToBinaryString(y);      // bin_y.size() == sizeof(float)
    std::string bin_s = ToBinaryString(s);      // bin_s.size() == sizeof(MyStruct)

    // Print sizes
    std::cout << "int: " << bin_x.size() << " bytes\n";
    std::cout << "float: " << bin_y.size() << " bytes\n";
    std::cout << "MyStruct: " << bin_s.size() << " bytes\n";

    // Note: The binary strings contain non-printable characters
}
```

**Notes:**

## Example: BLE Characteristic with Binary Float Value

You can use `ToBinaryString` to expose sensor values (such as a float) as raw binary data via a BLE characteristic. This is useful for transmitting precise values without string conversion overhead.

```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>
#include <CustomBLE/DataConversion.hpp>

// Assume you have a sensor object with a readCurrentAmperes() method returning float
extern MotorCurrentSensor getriebemotorCurrentSense;
ble_uuid128_t motorCurrentUUID = {/* ... */};

ServiceManager service_manager;
std::shared_ptr<Service> metexonBLEService = service_manager.emplace_service(/* service UUID */);

// Add a characteristic that returns the current as raw binary float
metexonBLEService->emplace_characteristic(
    motorCurrentUUID,
    []() { return ToBinaryString<float>(getriebemotorCurrentSense.readCurrentAmperes()); }
);
```

**Note:** The BLE client must interpret the characteristic value as a 4-byte IEEE 754 float. **No endianess conversion is performed!**, so ensure the client reads it correctly based on the platform's endianness.

## Quick Start: Pointer-Based Characteristics

### Read-Only Characteristic from Pointer
```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>

auto read_cb = Characteristic::make_pointer_read_callback(&sensor_value);
ble_uuid128_t char_uuid = {/* ... */};

ServiceManager service_manager;
std::shared_ptr<Service> service = service_manager.emplace_service(service_uuid);

// Method 1: Using callback makers with emplace
auto read_cb = Characteristic::make_pointer_read_callback(&sensor_value);
service->emplace_characteristic(char_uuid, read_cb);

// Method 2: Using factory method to create complete characteristic
auto characteristic = Characteristic::from_pointer_read_only(char_uuid, &sensor_value);
service->add_characteristic(std::move(characteristic));
```

### Read-Write Characteristic from Pointer
```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>

auto read_cb = Characteristic::make_pointer_read_callback(&config_value);
auto write_cb = Characteristic::make_pointer_write_callback(&config_value);

ServiceManager service_manager;
std::shared_ptr<Service> service = service_manager.emplace_service(service_uuid);

// Method 1: Using callback makers with emplace
auto read_cb = Characteristic::make_pointer_read_callback(&config_value);
auto write_cb = Characteristic::make_pointer_write_callback(&config_value);
service->emplace_characteristic(char_uuid, read_cb, write_cb);

// Method 2: Using factory method to create complete characteristic
auto characteristic = Characteristic::from_pointer_read_write(char_uuid, &config_value);
service->add_characteristic(std::move(characteristic));
```

### Write-Only Characteristic from Pointer
```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>

int command_value = 0;
ble_uuid128_t char_uuid = {/* ... */};

ServiceManager service_manager;
std::shared_ptr<Service> service = service_manager.emplace_service(service_uuid);

// Create write-only characteristic directly from pointer
auto characteristic = Characteristic::from_pointer_write_only(char_uuid, &command_value);
service->add_characteristic(std::move(characteristic));
```

## Comprehensive Usage Examples

### 1. Adding Characteristics Inline (Emplace)

```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>
#include <CustomBLE/Characteristic.hpp>

auto read_cb = [&]() { return my_value; };
auto write_cb = [&](const std::string& v) { my_value = v; };
std::shared_ptr<Service> service = service_manager.emplace_service(service_uuid);

// Add a characteristic with custom read/write callbacks
ble_uuid128_t char_uuid = {/* ... */};
std::string my_value = "Initial";
auto read_cb = [&]() { return my_value; };
auto write_cb = [&](const std::string& v) { my_value = v; };
service->emplace_characteristic(char_uuid, read_cb, write_cb);
```

### 2. Using Manager Classes Directly

```cpp
#include <CustomBLE/CharacteristicsManager.hpp>

CharacteristicsManager char_manager;
// char_manager.emplace_characteristic(char_uuid, "Value"); // Update to new API as needed
ble_gatt_chr_def* chr_defs = char_manager.get_chr_defs();
```

### 3. Full Service Setup and Registration

```cpp
#include <CustomBLE/ServiceManager.hpp>
#include <CustomBLE/Service.hpp>

ServiceManager service_manager;
ble_uuid128_t service_uuid = {/* ... */};
std::shared_ptr<Service> service = service_manager.emplace_service(service_uuid);

// Add multiple characteristics
// service->emplace_characteristic(char_uuid1, "A"); // Update to new API as needed
// service->emplace_characteristic(char_uuid2, "B"); // Update to new API as needed

// Register all services with NimBLE
service_manager.add_services_to_nimble();
```

### 4. Advanced: ServiceCharacteristicsManager


```cpp
#include <CustomBLE/ServiceCharacteristicsManager.hpp>

ServiceCharacteristicsManager adv_manager;
adv_manager.emplace_characteristic(char_uuid, read_cb, write_cb);
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

service->emplace_characteristic(char_uuid, read_cb, write_cb);
```

#### Fixed Value Callbacks
```cpp
#include <CustomBLE/GenericCallbacks.hpp>
#include <CustomBLE/Characteristic.hpp>

std::string my_value = "Initial";
auto read_cb = make_fixed_read_callback(my_value);
auto write_cb = make_fixed_write_callback(my_value);

service->emplace_characteristic(char_uuid, read_cb, write_cb);
```

## Notes
    - All manager classes support emplace-like methods for inline construction.
    - All service and characteristic creation methods now return `std::shared_ptr` objects. Use `->` to access their methods.
    - Use the `CustomBLE` namespace for all types.
    - See ESP-IDF NimBLE documentation for registration details.

## Generating BLE UUID Macros

To easily generate a C++ macro for a 128-bit BLE UUID, use the provided script:

```sh
python3 generate_bluetooth_uuid.py
```

This will output a line starting with `BLE_UUID128_INIT(` containing the UUID bytes in the correct format for use in your code. No parameters are required.
