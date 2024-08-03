# CMakeLists.txt for render

# File lists
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Create a library named 'render'
add_library(render ${SOURCES} ${HEADERS})

# Set include directories for this library
target_include_directories(render
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:include>
)

# Link against DirectX libraries
target_link_libraries(render
    PRIVATE
        d3d12
        dxgi
        d3dcompiler
        dxguid
)