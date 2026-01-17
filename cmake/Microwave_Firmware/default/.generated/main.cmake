# cmake files support debug production
include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(Microwave_Firmware_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(Microwave_Firmware_default_default_XC8_FILE_TYPE_assemble)
add_library(Microwave_Firmware_default_default_XC8_assemble OBJECT ${Microwave_Firmware_default_default_XC8_FILE_TYPE_assemble})
    Microwave_Firmware_default_default_XC8_assemble_rule(Microwave_Firmware_default_default_XC8_assemble)
    list(APPEND Microwave_Firmware_default_library_list "$<TARGET_OBJECTS:Microwave_Firmware_default_default_XC8_assemble>")

endif()

# Handle files with suffix S, for group default-XC8
if(Microwave_Firmware_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(Microwave_Firmware_default_default_XC8_assemblePreprocess OBJECT ${Microwave_Firmware_default_default_XC8_FILE_TYPE_assemblePreprocess})
    Microwave_Firmware_default_default_XC8_assemblePreprocess_rule(Microwave_Firmware_default_default_XC8_assemblePreprocess)
    list(APPEND Microwave_Firmware_default_library_list "$<TARGET_OBJECTS:Microwave_Firmware_default_default_XC8_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC8
if(Microwave_Firmware_default_default_XC8_FILE_TYPE_compile)
add_library(Microwave_Firmware_default_default_XC8_compile OBJECT ${Microwave_Firmware_default_default_XC8_FILE_TYPE_compile})
    Microwave_Firmware_default_default_XC8_compile_rule(Microwave_Firmware_default_default_XC8_compile)
    list(APPEND Microwave_Firmware_default_library_list "$<TARGET_OBJECTS:Microwave_Firmware_default_default_XC8_compile>")

endif()


# Main target for this project
add_executable(Microwave_Firmware_default_image_z56d5qOU ${Microwave_Firmware_default_library_list})

set_target_properties(Microwave_Firmware_default_image_z56d5qOU PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Microwave_Firmware_default_output_dir})
set_target_properties(Microwave_Firmware_default_image_z56d5qOU PROPERTIES OUTPUT_NAME "default")
set_target_properties(Microwave_Firmware_default_image_z56d5qOU PROPERTIES SUFFIX ".elf")

target_link_libraries(Microwave_Firmware_default_image_z56d5qOU PRIVATE ${Microwave_Firmware_default_default_XC8_FILE_TYPE_link})


# Add the link options from the rule file.
Microwave_Firmware_default_link_rule(Microwave_Firmware_default_image_z56d5qOU)



