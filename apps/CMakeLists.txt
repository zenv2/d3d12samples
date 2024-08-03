# CMakeLists.txt for apps folder

# List all subdirectories (executables)
file(GLOB APPS "${CMAKE_CURRENT_SOURCE_DIR}/*")

# Iterate through each subdirectory and add it as an executable
foreach(APP_DIR ${APPS})
    if(IS_DIRECTORY ${APP_DIR})
        get_filename_component(APP_NAME ${APP_DIR} NAME)
        
        file(GLOB INPUT_FILES "${APP_DIR}/shaders/*.hlsl")
        foreach(INPUT_FILE ${INPUT_FILES})
            get_filename_component(INPUT_FILE_NAME ${INPUT_FILE} NAME_WE)
            file(READ ${INPUT_FILE} FILE_CONTENT)
            configure_file(
                ${CMAKE_CURRENT_SOURCE_DIR}/shaders_file.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}/fx_${INPUT_FILE_NAME}.cpp
                @ONLY
            )

            configure_file(
                ${CMAKE_CURRENT_SOURCE_DIR}/shaders_file.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}/fx_${INPUT_FILE_NAME}.h
                @ONLY
            )
        endforeach()

        add_subdirectory(${APP_NAME})
    endif()
endforeach()
