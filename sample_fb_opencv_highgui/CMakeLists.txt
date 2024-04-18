cmake_minimum_required(VERSION 3.16) 
PROJECT (sample_fb_opencv_highgui)

set (CMAKE_CXX_STANDARD 11)


find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()



file(GLOB TARGET_SRC "*.c*")
file(GLOB TARGET_HD "*.h*")

include_directories( ${OpenCV_INCLUDE_DIRS} )


add_executable( sample_fb_opencv_highgui ${TARGET_SRC} ${TARGET_HD} )

target_link_libraries(sample_fb_opencv_highgui ${OpenCV_LIBS} )
target_link_libraries(sample_fb_opencv_highgui OpenMP::OpenMP_CXX)

