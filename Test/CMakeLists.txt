# Standalone serializer conformance — corpus byte-exactness + UE frame
# conversion + deriver checks. No engine / UE4SS required; guards the
# vendored core (OgsTelemetry/*.h) against the standard's corpus:
#   cmake -S Test -B Test/build && cmake --build Test/build
#   Test/build/ogs_conformance ../../OGS-Telemetry-Standard/conformance
cmake_minimum_required(VERSION 3.16)
project(ogs_conformance CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(ogs_conformance conformance.cpp)
target_include_directories(ogs_conformance PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(WIN32)
  add_executable(ogs_smoke smoke.cpp ../plugin/ogs_publisher.cpp)
  set_target_properties(ogs_smoke PROPERTIES CXX_STANDARD 20)
  target_include_directories(ogs_smoke PRIVATE ../plugin ..)
  target_link_libraries(ogs_smoke ws2_32)
endif()
