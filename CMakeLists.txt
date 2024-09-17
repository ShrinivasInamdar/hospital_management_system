cmake_minimum_required(VERSION 3.5.0)
project(hospitalmanagement VERSION 0.1.0 LANGUAGES C CXX)

# Add SQLite3 as a static library
add_library(sqlite3 STATIC lib/sqlite3.c)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


# Add the source files for the project
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/patientmodule.cpp
    ${CMAKE_SOURCE_DIR}/src/doctormodule.cpp
    ${CMAKE_SOURCE_DIR}/src/hospitalmanagement.cpp
)

# Add the executable (the main program to be built)
add_executable(hospitalmanagement ${SOURCES})

# Link SQLite3 to the hospitalmanagement executable
target_link_libraries(hospitalmanagement PRIVATE sqlite3)

# Include directories for headers
target_include_directories(hospitalmanagement PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(hospitalmanagement PRIVATE ${CMAKE_SOURCE_DIR}/include)
