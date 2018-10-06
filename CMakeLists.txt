cmake_minimum_required(VERSION 2.8)                         
project( WhitePatternDetection )                                                               
find_package( OpenCV REQUIRED )                            
add_executable( WhitePatternDetection WhitePatternDetection.cpp)               
target_link_libraries( WhitePatternDetection ${OpenCV_LIBS} )  
#include_directories($ENV{AC_DIR})

      