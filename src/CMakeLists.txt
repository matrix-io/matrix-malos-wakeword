cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11)

include (../cmake/FatalWarnings.cmake)
ADM_EXTRA_WARNINGS()

find_package(Protobuf 3 REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

FIND_LIBRARY(ZMQ_LIB NAMES zmq)
FIND_LIBRARY(MATRIX_LIB NAMES matrix_malos)
FIND_LIBRARY(MATRIX_ZMQ_LIB NAMES matrix_malos_zmq)
FIND_LIBRARY(MATRIX_PROTOS_LIB NAMES matrixio_protos)

pkg_check_modules(POCKETSPHINX REQUIRED pocketsphinx)
pkg_check_modules(SPHINXBASE REQUIRED sphinxbase)

message(STATUS "ZMQ found => " "${ZMQ_LIB}")
message(STATUS "MATRIX LIB found => " "${MATRIX_LIB}")
message(STATUS "MATRIX ZMQ LIB found => " "${MATRIX_ZMQ_LIB}")
message(STATUS "MATRIXIO PROTOS=> " "${MATRIX_PROTOS_LIB}")
message(STATUS "SPHINXBASE_LIBRARIES => " "${SPHINXBASE_LIBRARIES}")
message(STATUS "POCKETSPHINX_LIBRARIES => " "${POCKETSPHINX_LIBRARIES}")
message(STATUS "POCKETSPHINX_INCLUDE_DIRS => " "${POCKETSPHINX_INCLUDE_DIRS}")
message(STATUS "SPHINXBASE_INCLUDE_DIRS => " "${SPHINXBASE_INCLUDE_DIRS}")

set(
  malos_wakeword_src
  malos_wakeword.cpp
  driver_wakeword.cpp
)

add_executable(malos_wakeword ${malos_wakeword_src})
  set_property(TARGET malos_wakeword PROPERTY CXX_STANDARD 11)
  target_link_libraries(malos_wakeword ${PROTOBUF_LIBRARY})
  target_link_libraries(malos_wakeword ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(malos_wakeword ${MATRIX_LIB})
  target_link_libraries(malos_wakeword ${MATRIX_PROTOS_LIB})
  target_link_libraries(malos_wakeword ${MATRIX_ZMQ_LIB} ${ZMQ_LIB})

add_executable(malos_psphinx malos_psphinx.cpp)
  set_property(TARGET malos_psphinx PROPERTY CXX_STANDARD 11)  
  target_include_directories(malos_psphinx PUBLIC ${POCKETSPHINX_INCLUDE_DIRS})
  target_include_directories(malos_psphinx PUBLIC ${SPHINXBASE_INCLUDE_DIRS})
  target_compile_options(malos_psphinx PUBLIC ${POCKETSPHINX_CFLAGS_OTHER})
  target_compile_options(malos_psphinx PUBLIC ${SPHINXBASE_CFLAGS_OTHER})
  target_link_libraries(malos_psphinx ${SPHINXBASE_LIBRARIES})
  target_link_libraries(malos_psphinx ${POCKETSPHINX_LIBRARIES})

# Binary to be installed.
install(TARGETS malos_wakeword DESTINATION bin)
install(TARGETS malos_psphinx DESTINATION bin)

