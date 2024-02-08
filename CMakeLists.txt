cmake_minimum_required(VERSION 3.15)
project(SignalHoundStream)

# Set C++ version
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# Download and include nlohmann/json
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2  # You can replace this with the desired version or 'master' to always use the latest version
)
FetchContent_MakeAvailable(json)

# Add your source files here
set(SOURCE_FILES
src/main.cpp
src/SignalHoundStream.cpp
)

# Add header files if needed
set(HEADER_FILES
src/SignalHoundStream.h
)

# Set Visual Studio 2019 generator
if (CMAKE_GENERATOR MATCHES "Visual Studio")
set(CMAKE_GENERATOR_TOOLSET "host=x64")
set(CMAKE_GENERATOR_PLATFORM "x64")
set(CMAKE_GENERATOR_VERSION 16)
endif()

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json )

# Copy DLL and LIB to build directory before the main target is built
add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/sm_api.dll ${CMAKE_BINARY_DIR}/sm_api.lib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/signal_hound_sdk/device_apis/sm_series/lib/win/vs2019/x64/sm_api.dll"
        ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/signal_hound_sdk/device_apis/sm_series/lib/win/vs2019/x64/sm_api.lib"
        ${CMAKE_BINARY_DIR}
        COMMENT "Copying sm_api.dll and sm_api.lib to build directory"
)

# Add a custom target to trigger copying before the main target is built
add_custom_target(CopyLIB DEPENDS ${CMAKE_BINARY_DIR}/sm_api.dll ${CMAKE_BINARY_DIR}/sm_api.lib)

# Make the main target depend on the custom target
add_dependencies(${PROJECT_NAME} CopyLIB)

# Signal Hound include directory
include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/signal_hound_sdk/device_apis/sm_series/include/)

# Copy DLL to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/signal_hound_sdk/device_apis/sm_series/lib/win/vs2019/x64/sm_api.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )