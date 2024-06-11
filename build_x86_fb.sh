#!/bin/bash

WORK_DIR=$PWD

function build_version() {
  OPENCV_BUILD_DIR=$1
  OPENCV_INSTALL_DIR=$2

  echo "OpenCV build directory:   $WORK_DIR/${OPENCV_BUILD_DIR}"
  echo "OpenCV install directory: $WORK_DIR/${OPENCV_INSTALL_DIR}"

  mkdir $OPENCV_BUILD_DIR
  mkdir $OPENCV_INSTALL_DIR

  cd $WORK_DIR/$OPENCV_BUILD_DIR
  echo "---------------------------------------"
  echo "Build OpenCV"
  echo "Working directory:        ${PWD}"
  cmake \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_OPENCL=OFF \
    -DWITH_IPP=OFF \
    -DWITH_OPENMP=ON \
    -DWITH_TBB=OFF \
    -DWITH_PTHREADS_PF=OFF \
    -DWITH_VA=OFF \
    -DWITH_VA_INTEL=OFF \
    -DBUILD_JAVA=OFF \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_ZLIB=ON \
    -DBUILD_PNG=ON \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DWITH_FRAMEBUFFER=ON \
    -DBUILD_opencv_python2=OFF \
    -DBUILD_opencv_python3=OFF \
    -DWITH_GTK=OFF \
    -DCMAKE_INSTALL_PREFIX=$WORK_DIR/$OPENCV_INSTALL_DIR \
    $WORK_DIR/opencv_framebuffer
    make -j4
    make install

  echo "---------------------------------------"
}



#
echo "======================================="
echo "Build opencv with framebuffer for riscv process"
echo "---------------------------------------"
build_version opencv-build-fb-x86 opencv-install-fb-x86 
echo "======================================="

