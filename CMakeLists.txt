
idf_component_register(
    SRCS "src/Characteristic.cpp"
         "src/CharacteristicsManager.cpp"
         "src/Service.cpp"
         "src/ServiceCharacteristicsManager.cpp"
         "src/ServiceManager.cpp"
    INCLUDE_DIRS "include"
    REQUIRES bt
)
