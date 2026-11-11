cmake_minimum_required(VERSION 3.21)

project(ClinicQueue LANGUAGES CXX)

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(clinic_queue
    src/main.cpp
    src/doctor_repository.cpp
    src/ticket_repository.cpp
    src/sqlite_api_demo.cpp
    src/demo.cpp
)

target_include_directories(clinic_queue PRIVATE include)
target_compile_features(clinic_queue PRIVATE cxx_std_20)
target_link_libraries(clinic_queue PRIVATE
    sqlite_orm::sqlite_orm
    unofficial::sqlite3::sqlite3
)

if(MSVC)
    target_compile_options(clinic_queue PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(clinic_queue PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_property(TARGET clinic_queue PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
