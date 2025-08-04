
#pragma once
#include "CustomBLEService.hpp"
#include <vector>
#include <memory>
#include <cstddef>

namespace CustomBLE {

class ServiceManager {
private:
    std::vector<std::unique_ptr<Service>> services;
    std::vector<ble_gatt_svc_def> svc_defs;

public:
    void add_service(std::unique_ptr<Service> service);

    /**
     * @brief Emplace a new service inline (constructs and adds).
     * @param uuid UUID of the service
     * @return Reference to the newly added Service
     */
    Service& emplace_service(const ble_uuid128_t& uuid);
    ble_gatt_svc_def* get_svc_defs();
    size_t size() const;

private:
    void update_svc_defs();
};

} // namespace CustomBLE
