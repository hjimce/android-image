cmake_minimum_required(VERSION 3.5)
project(mxnet_faceattribute)
#find_package(OpenMP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")


option (USE_SYSTEM_OPENCV
       "Use SYSTEM OPENCV" OFF)
if(USE_SYSTEM_OPENCV)
	find_package( OpenCV REQUIRED )
	INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
else()
	SET(OPENCV_INCLUDE "../../../opencv/xcode/OpenCV-3.1.0-335-g05384f0-x86_64/include") 
	SET(OPENCV_LIB "../../../opencv/xcode/OpenCV-3.1.0-335-g05384f0-x86_64/lib/") 
	INCLUDE_DIRECTORIES(${OPENCV_INCLUDE})
	LINK_DIRECTORIES("../../../opencv/xcode/lib/")
endif()

set(USE_CUDA OFF)
set(USE_CUDNN OFF)

#add_subdirectory("./3dparty/mxnet/")
include_directories("./include")
link_directories(.)


set(sourcefile  src/c_predict_api.h src/CFaceAttribute.cpp src/mutil_task.cpp src/linux/mxnet_predict-all.cc)
add_library(faceattribute SHARED ${sourcefile})
add_executable(predict_race_eyes example/main.cpp example/test_unit.h)
add_executable(predict_light_mouth example/test_mutiltask.cpp example/test_unit.h)
if(USE_SYSTEM_OPENCV)
	target_link_libraries(faceattribute  ${OpenCV_LIBS} )
	target_link_libraries(predict_race_eyes  ${OpenCV_LIBS} faceattribute)
	target_link_libraries(predict_light_mouth  ${OpenCV_LIBS} faceattribute)
else()
	target_link_libraries(faceattribute  opencv_world.3.1.0.dylib )
	target_link_libraries(predict_race_eyes  opencv_world.3.1.0.dylib faceattribute)
	target_link_libraries(predict_light_mouth  opencv_world.3.1.0.dylib faceattribute)
endif()


