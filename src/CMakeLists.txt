FetchContent_MakeAvailable(ilo)

configure_file (
    "${PROJECT_SOURCE_DIR}/src/mmtaudioparser_config.h.in"
    "${PROJECT_SOURCE_DIR}/include/mmtaudioparser/version.h"
)

add_library(mmtaudioparser STATIC
    ${PROJECT_SOURCE_DIR}/include/mmtaudioparser/mmtaudioparser.h
    ${PROJECT_SOURCE_DIR}/include/mmtaudioparser/mpeghparser.h
    ${PROJECT_SOURCE_DIR}/include/mmtaudioparser/version.h
    logging.h
    mpeghparser.cpp
    mpeghparserpimpl.cpp
    mpeghparserpimpl.h
    parserutils.h
    parserutils.cpp
)

target_compile_features(mmtaudioparser PUBLIC cxx_std_11)
set_target_properties(mmtaudioparser PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(mmtaudioparser PUBLIC ${PROJECT_SOURCE_DIR}/include/)
target_link_libraries(mmtaudioparser PUBLIC ilo)

if(EMSCRIPTEN)
  # Enable C++ exception support for WASM
  target_compile_options(mmtaudioparser PUBLIC "-fexceptions")
  target_link_options(mmtaudioparser PUBLIC "-fexceptions")
endif()
