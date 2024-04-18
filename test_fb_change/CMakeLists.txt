cmake_minimum_required(VERSION 3.16) 
PROJECT (test_fb_change)

set (CMAKE_CXX_STANDARD 11)


find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()



file(GLOB TARGET_SRC "*.c*")
file(GLOB TARGET_HD "*.h*")

include_directories( ${OpenCV_INCLUDE_DIRS} )


add_executable( test_fb_change ${TARGET_SRC} ${TARGET_HD} )

target_link_libraries(test_fb_change ${OpenCV_LIBS} )
target_link_libraries(test_fb_change OpenMP::OpenMP_CXX)

