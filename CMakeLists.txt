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

option(SKIP_VCPKG_INSTALL "Do not run vcpkg install during configure" OFF)
set(_stamp_file "${CMAKE_BINARY_DIR}/.vcpkg_manifest_stamp")

if(NOT SKIP_VCPKG_INSTALL AND EXISTS "${CMAKE_SOURCE_DIR}/vcpkg.json")
  file(TIMESTAMP "${CMAKE_SOURCE_DIR}/vcpkg.json" _manifest_ts)
  if(EXISTS "${_stamp_file}")
    file(READ "${_stamp_file}" _last_ts)
  else()
    set(_last_ts "")
  endif()

  if(NOT _manifest_ts STREQUAL _last_ts)
    # find vcpkg (same as option A)
    if(DEFINED ENV{VCPKG_ROOT})
      set(_vcpkg_exe "$ENV{VCPKG_ROOT}/vcpkg")
      if(WIN32)
        set(_vcpkg_exe "${_vcpkg_exe}.exe")
      endif()
    else()
      find_program(_vcpkg_exe NAMES vcpkg HINTS $ENV{VCPKG_ROOT})
    endif()
    if(NOT _vcpkg_exe)
      find_program(_vcpkg_exe NAMES vcpkg)
    endif()
    if(NOT _vcpkg_exe)
      message(FATAL_ERROR "vcpkg not found. Set VCPKG_ROOT or put vcpkg on PATH or set SKIP_VCPKG_INSTALL=ON")
    endif()

    message(STATUS "vcpkg.json changed or first run; running: ${_vcpkg_exe} install --manifest")
    execute_process(
      COMMAND "${_vcpkg_exe}" install
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      RESULT_VARIABLE _vcpkg_res
      ERROR_VARIABLE _vcpkg_err
    )
    if(NOT _vcpkg_res EQUAL 0)
      message(FATAL_ERROR "vcpkg install failed:\n${_vcpkg_err}")
    endif()

    # update stamp file with manifest timestamp
    file(WRITE "${_stamp_file}" "${_manifest_ts}")
  else()
    message(STATUS "vcpkg manifest unchanged; skipping vcpkg install")
  endif()
endif()

if(NOT DEFINED VCPKG_TRIPLET)
  if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET})
    set(VCPKG_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}")
  elseif(WIN32)
    set(VCPKG_TRIPLET "x64-windows")
  else()
    set(VCPKG_TRIPLET "x64-linux")
  endif()
endif()

set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed/${VCPKG_TRIPLET}")

add_executable(snake ${SNAKE_SOURCES})

# For header-only include path
target_include_directories(snake PRIVATE "${VCPKG_INSTALLED_DIR}/include")

# For linking static libs you can point to lib or lib64 accordingly:
target_link_directories(snake PRIVATE "${VCPKG_INSTALLED_DIR}/lib")
# target_link_libraries(snake PRIVATE some_library_name)

target_include_directories(snake PRIVATE "${CMAKE_SOURCE_DIR}/src/include")
