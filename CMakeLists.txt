cmake_minimum_required(VERSION 3.9)
project(protobuf_service_demo)

set(CMAKE_CXX_STANDARD 11)
#something wrong so I remove this
SET(CMAKE_CXX_FLAGS "-DSOFA_PBRPC_PROFILING")

find_package(Protobuf REQUIRED)

include_directories(include)
add_library(service include/service.pb.cc)

#add sofa-pbrpc
include_directories(/exapp/pengjiawei/sofa-pbrpc/output/include)
link_directories(/exapp/pengjiawei/sofa-pbrpc/output/lib)

link_libraries(sofa-pbrpc ${PROTOBUF_LIBRARIES} snappy pthread z service)

add_executable(client_demo src/client_demo.cpp)
target_link_libraries(client_demo sofa-pbrpc )

add_executable(server_demo src/server_demo.cpp)
target_link_libraries(server_demo sofa-pbrpc libtcmalloc_and_profiler.a unwind)

add_executable(mock_test mock_test/mock_test.cpp)
target_link_libraries(mock_test gtest sofa-pbrpc)

add_executable(http_client_demo src/http_client_demo.cpp)
target_link_libraries(http_client_demo curlpp pthread curl)