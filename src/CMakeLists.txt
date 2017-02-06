cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11)

include (../cmake/FatalWarnings.cmake)
ADM_EXTRA_WARNINGS()

find_package(Protobuf 3 REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig)

# adding sphinx libraries via pkgconfig
pkg_search_module(SPHINXBASE REQUIRED sphinxbase)
pkg_search_module(POCKETSPHINX REQUIRED pocketsphinx)

# This won't find ZQM in custom locations.
# FIXME(nelson.castillo): Improve.
FIND_LIBRARY(ZMQ_LIB NAMES zmq)
FIND_LIBRARY(HAL_LIB NAMES matrix_creator_hal)
FIND_LIBRARY(MATRIX_LIB NAMES matrix_malos)
FIND_LIBRARY(MATRIX_ZMQ_LIB NAMES matrix_malos_zmq)

message(STATUS "ZMQ found => " "${ZMQ_LIB}")
message(STATUS "HAL found => " "${HAL_LIB}")
message(STATUS "MATRIX LIB found => " "${MATRIX_LIB}")
message(STATUS "MATRIX ZMQ LIB found => " "${MATRIX_ZMQ_LIB}")

PROTOBUF_GENERATE_CPP(MALOS_PROTO_SRC MALOS_PROTO_HEADER ../protocol-buffers/malos/driver.proto)

include_directories(
  ${PROJECT_BINARY_DIR} # Protos.
  ${PROTOBUF_INCLUDE_DIRS})

set(malos_wakeword
  malos_wakeword.cpp
  driver_manager.cpp driver_manager.h
  driver_wakeword.cpp driver_wakeword.h
  ${MALOS_PROTO_SRC} ${MALOS_PROTO_HEADER}
)

add_executable(malos_wakeword ${malos_wakeword})
  set_property(TARGET malos_wakeword PROPERTY CXX_STANDARD 11)
  target_link_libraries(malos_wakeword ${PROTOBUF_LIBRARY})
  target_link_libraries(malos_wakeword ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(malos_wakeword ${MATRIX_LIB})
  target_link_libraries(malos_wakeword ${MATRIX_ZMQ_LIB} ${ZMQ_LIB})
  target_link_libraries(malos_wakeword ${SPHINXBASE_LIBRARIES})
  target_link_libraries(malos_wakeword ${POCKETSPHINX_LIBRARIES})

# Binary to be installed.
install(TARGETS malos_wakeword DESTINATION bin)
