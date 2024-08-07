cmake_minimum_required(VERSION 3.16)
include(FetchContent)

include_guard()

project(mmtaudioparser VERSION 1.0.0)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(mmtaudioparser_BUILD_DOC  OFF CACHE BOOL "Build doxygen doc")

FetchContent_Declare(
  ilo
  GIT_REPOSITORY https://github.com/Fraunhofer-IIS/ilo.git
  GIT_TAG        r1.0.0
)

add_subdirectory(src)

if(mmtaudioparser_BUILD_DOC)
  add_subdirectory(doc)
endif()
