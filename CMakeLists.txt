cmake_minimum_required(VERSION 3.9)
project(protobuf_service_demo)

set(CMAKE_CXX_STANDARD 11)
find_package(Protobuf REQUIRED)

add_library(service service.pb.cc)

#add sofa-pbrpc
include_directories(/exapp/pengjiawei/sofa-pbrpc/output/include)
link_directories(/exapp/pengjiawei/sofa-pbrpc/output/lib)

add_executable(client_demo client_demo.cpp)
target_link_libraries(client_demo sofa-pbrpc ${PROTOBUF_LIBRARIES}  snappy pthread z service)

add_executable(server_demo server_demo.cpp)
target_link_libraries(server_demo ${PROTOBUF_LIBRARIES} sofa-pbrpc snappy pthread z service)