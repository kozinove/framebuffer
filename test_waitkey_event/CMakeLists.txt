cmake_minimum_required(VERSION 3.16) 
PROJECT (test_waitkey_with_event)

set (CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB TARGET_SRC "*.c*")
file(GLOB TARGET_HD "*.h*")

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( test_waitkey_with_event ${TARGET_SRC} ${TARGET_HD} )

target_link_libraries(test_waitkey_with_event ${OpenCV_LIBS} )