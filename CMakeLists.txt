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

FetchContent_Declare(webpp
    GIT_REPOSITORY https://git.jcm.re/jcm/webpp.git
    GIT_TAG main
)
FetchContent_Declare(webxx
    GIT_REPOSITORY https://github.com/rthrfrd/webxx.git
    GIT_TAG v0.9.2
)
FetchCOntent_Declare(tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG v10.0.0)

set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(webpp_BUILD_STD_MODULE OFF CACHE BOOL "" FORCE)
add_compile_options(-fno-rtti -fno-exceptions)
FetchContent_MakeAvailable(webpp webxx tinyxml2)

add_library(webxxModule STATIC)
target_sources(webxxModule PUBLIC FILE_SET CXX_MODULES FILES third_party/webxx.cppm)
target_compile_features(webxxModule PUBLIC cxx_std_26)
target_link_libraries(webxxModule PUBLIC webxx::webxx)

add_library(tinyxml2Module STATIC)
target_sources(tinyxml2Module PUBLIC FILE_SET CXX_MODULES FILES third_party/tinyxml2.cppm)
target_compile_features(tinyxml2Module PUBLIC cxx_std_26)
target_link_libraries(tinyxml2Module PUBLIC tinyxml2::tinyxml2)

add_library(stdModule STATIC)
target_sources(stdModule PUBLIC FILE_SET CXX_MODULES BASE_DIRS /usr/lib/llvm-20/share/libc++/v1/
    FILES /usr/lib/llvm-20/share/libc++/v1/std.cppm)
target_compile_features(stdModule PUBLIC cxx_std_26)
# The standard library may use reserved identifiers
target_compile_options(stdModule PRIVATE -Wno-reserved-identifier)
# These two are not supported by WASI, so we just define their header guards, so they will be skipped
target_compile_definitions(stdModule PRIVATE _LIBCPP_CSETJMP _LIBCPP_CSIGNAL)

set(SOURCES
    src/main.cpp
    src/c++support.cpp
)
set(MODULE_SOURCES
    src/components.cppm
    src/utils.cppm
    src/c_interpreter.cppm
)

add_executable(website ${SOURCES})
target_sources(website PUBLIC FILE_SET CXX_MODULES BASE_DIRS src FILES ${MODULE_SOURCES})
target_compile_features(website PRIVATE cxx_std_26)
target_link_options(website PRIVATE -Wl,--no-entry)
target_link_options(website PRIVATE -nostartfiles)
target_link_libraries(website PRIVATE stdModule webxxModule tinyxml2Module webpp::webpp)
target_compile_options(website PRIVATE --embed-dir=${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(website PRIVATE -Wno-c23-extensions)

FetchContent_GetProperties(webpp)
add_custom_command(
    COMMAND ${CMAKE_COMMAND} -E copy ${webpp_SOURCE_DIR}/webpp.js ${CMAKE_CURRENT_BINARY_DIR}/webpp.js
    DEPENDS ${webpp_SOURCE_DIR}/webpp.js
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/webpp.js
)
add_custom_target(copy_webpp_js ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/webpp.js)
