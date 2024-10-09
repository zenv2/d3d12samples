# CMakeLists.txt for depthtest app

# List all source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_BINARY_DIR}/fx_*.cpp")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Define the executable
add_executable(depthtest ${SOURCES} ${HEADERS})

# Link with libraries
target_link_libraries(depthtest
    PRIVATE
        render
        d3d12
        dxgi
        d3dcompiler
        dxguid
)

# Set include directories for this executable (if needed)
target_include_directories(depthtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
