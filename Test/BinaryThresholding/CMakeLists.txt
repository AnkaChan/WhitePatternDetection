cmake_minimum_required(VERSION 2.8)                         
project( BinaryThresholding )                                                               
find_package( OpenCV REQUIRED )                            
add_executable( BinaryThresholding BinaryThresholding.cpp)               
target_link_libraries( BinaryThresholding ${OpenCV_LIBS} )  
#include_directories($ENV{AC_DIR})

      