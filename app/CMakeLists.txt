cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
find_library(log-lib log )

include(src/main/cpp/face_detection/dlib/cmake)
link_directories("./src/main/cpp/NNPACK/obj/local/armeabi-v7a")
include_directories("./src/main/cpp/NNPACK/include")
include_directories("./src/main/cpp/NNPACK/deps/pthreadpool/include")

add_library( face_detection_static SHARED src/main/cpp/ndk_face_detection.cpp src/main/cpp/ndk_nnpack.cpp)
target_link_libraries(face_detection_static dlib libnnpack.a  libnnpack_ukernels.a libpthreadpool.a libcpufeatures.a ${log-lib} )

