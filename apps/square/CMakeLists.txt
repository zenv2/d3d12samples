# CMakeLists.txt for square app

# List all source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_BINARY_DIR}/fx_*.cpp")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Define the executable
add_executable(square ${SOURCES} ${HEADERS})

# Link with libraries
target_link_libraries(square
    PRIVATE
        render
        d3d12
        dxgi
        d3dcompiler
        dxguid
)

# Set include directories for this executable (if needed)
target_include_directories(square PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
