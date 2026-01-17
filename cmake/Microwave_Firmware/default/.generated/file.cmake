# The following variables contains the files used by the different stages of the build process.
set(Microwave_Firmware_default_default_XC8_FILE_TYPE_assemble)
set_source_files_properties(${Microwave_Firmware_default_default_XC8_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${Microwave_Firmware_default_default_XC8_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(Microwave_Firmware_default_default_XC8_FILE_TYPE_assemblePreprocess)
set_source_files_properties(${Microwave_Firmware_default_default_XC8_FILE_TYPE_assemblePreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${Microwave_Firmware_default_default_XC8_FILE_TYPE_assemblePreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(Microwave_Firmware_default_default_XC8_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../buzzer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../clcd.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../matrix_keypad.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../oven.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../timers.c")
set_source_files_properties(${Microwave_Firmware_default_default_XC8_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(Microwave_Firmware_default_default_XC8_FILE_TYPE_link)
set(Microwave_Firmware_default_image_name "default.elf")
set(Microwave_Firmware_default_image_base_name "default")

# The output directory of the final image.
set(Microwave_Firmware_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/Microwave_Firmware")

# The full path to the final image.
set(Microwave_Firmware_default_full_path_to_image ${Microwave_Firmware_default_output_dir}/${Microwave_Firmware_default_image_name})
