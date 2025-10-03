cmake_minimum_required(VERSION 3.20)

project(snake_game VERSION 0.1 LANGUAGES CXX)

# recommended defaults
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# put sources in src/ for now
file(GLOB_RECURSE SNAKE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(snake ${SNAKE_SOURCES})

add_custom_command(TARGET snake POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/config/logging.ini"
    "$<TARGET_FILE_DIR:snake>/logging.ini"
)

find_package(Boost REQUIRED COMPONENTS log log_setup)

# For header-only include path
# target_include_directories(snake PRIVATE "${VCPKG_INSTALLED_DIR}/include")

# For linking static libs you can point to lib or lib64 accordingly:
# target_link_directories(snake PRIVATE "${VCPKG_INSTALLED_DIR}/lib")
target_link_libraries(snake PRIVATE
    Boost::log
    Boost::log_setup
    ${CMAKE_DL_LIBS}  # Required for dynamic loading on Linux
)

target_include_directories(snake PRIVATE "${CMAKE_SOURCE_DIR}/src/engine/include")
