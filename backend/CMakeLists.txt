cmake_minimum_required(VERSION 3.16)
project(ChatForum LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

find_package(Drogon CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(unofficial-libmysql CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES
    "controllers/*.cpp"
    "models/*.cpp"
    "services/*.cpp"
    "utils/*.cpp"
    "websocket/*.cpp"
    "main.cpp"
)

add_executable(ChatForum ${SOURCES})

target_include_directories(ChatForum PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ChatForum PRIVATE
    Drogon::Drogon
    jwt-cpp::jwt-cpp
    unofficial::libmysql::mysqlclient
)

if(MSVC)
    target_compile_options(ChatForum PRIVATE /utf-8)
endif()
