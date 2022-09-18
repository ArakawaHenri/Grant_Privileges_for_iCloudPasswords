cmake_minimum_required (VERSION 3.23)
include(FetchContent)
project ("grant_privileges")

FetchContent_Declare(
  configor
  URL       https://github.com/Nomango/configor/archive/refs/tags/v0.9.19.zip
)

FetchContent_GetProperties(configor)
if (NOT configor_POPULATED)
  FetchContent_Populate(configor)
endif ()

add_library (configor INTERFACE)
include_directories(configor ${configor_SOURCE_DIR}/include)

add_subdirectory ("grant_privileges")
