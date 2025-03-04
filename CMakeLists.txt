cmake_minimum_required(VERSION 3.28)

project(website)

set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(CMAKE_SYSTEM_NAME WASI)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR wasm32)
set(TARGET_TRIPLE wasm32-wasi)

set(CMAKE_C_COMPILER_TARGET   ${TARGET_TRIPLE})
set(CMAKE_CXX_COMPILER_TARGET ${TARGET_TRIPLE})

set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
set(CMAKE_LINK_DEPENDS_USE_LINKER OFF)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_C_FLAGS_MINSIZEREL "-Oz")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Oz")

include(FetchContent)

FetchContent_Declare(webxx
    GIT_REPOSITORY https://github.com/rthrfrd/webxx.git
    GIT_TAG v0.9.2
)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(webxx json)

add_library(webxxModule STATIC)
target_sources(webxxModule PUBLIC FILE_SET CXX_MODULES FILES third_party/webxx.cppm)
target_compile_features(webxxModule PUBLIC cxx_std_23)
target_link_libraries(webxxModule PUBLIC webxx::webxx)

add_library(jsonModule STATIC)
target_sources(jsonModule PUBLIC FILE_SET CXX_MODULES FILES third_party/json.cppm)
target_compile_features(jsonModule PUBLIC cxx_std_23)
target_link_libraries(jsonModule PUBLIC nlohmann_json::nlohmann_json)

set(SOURCES
    src/main.cpp
    src/c++support.cpp
)
set(MODULE_SOURCES
    src/web.cppm
    src/web_coro.cppm
    src/utils.cppm
    src/c_interpreter.cppm
)

add_executable(website ${SOURCES})
target_sources(website PUBLIC FILE_SET CXX_MODULES BASE_DIRS src FILES ${MODULE_SOURCES})
target_compile_features(website PRIVATE cxx_std_23)
target_link_options(website PRIVATE -Wl,--no-entry)
target_link_options(website PRIVATE -nostartfiles)
target_link_libraries(website PRIVATE webxxModule jsonModule)
target_compile_options(website PRIVATE --embed-dir=${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(website PRIVATE -Wno-c23-extensions)
