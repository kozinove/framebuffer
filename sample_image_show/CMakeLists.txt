cmake_minimum_required(VERSION 3.16) 
PROJECT (image_show)

set (CMAKE_CXX_STANDARD 11)


find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()



file(GLOB TARGET_SRC "*.c*")
file(GLOB TARGET_HD "*.h*")

include_directories( ${OpenCV_INCLUDE_DIRS} )


add_executable( image_show ${TARGET_SRC} ${TARGET_HD} )

target_link_libraries(image_show ${OpenCV_LIBS} )
target_link_libraries(image_show OpenMP::OpenMP_CXX)

