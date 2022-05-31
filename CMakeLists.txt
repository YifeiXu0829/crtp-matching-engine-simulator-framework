cmake_minimum_required(VERSION 3.5)

if(POLICY CMP0048)
      cmake_policy(SET CMP0048 NEW)
endif()

project (projectname VERSION "1.1.0")
#configure_file(config.h.in config.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
find_package(Boost 1.78.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIRS})
add_executable(main main.cpp)
target_link_libraries(main ${YAML_CPP_LIBRARIES})
