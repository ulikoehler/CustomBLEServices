
idf_component_register(
    SRCS "src/CustomBLE/Characteristic.cpp"
         "src/CustomBLE/CharacteristicsManager.cpp"
         "src/CustomBLE/Service.cpp"
         "src/CustomBLE/ServiceManager.cpp"
    INCLUDE_DIRS "include"
    REQUIRES bt
)
