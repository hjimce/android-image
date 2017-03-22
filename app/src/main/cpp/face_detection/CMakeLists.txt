#set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
#SET(OpenCV_INCLUDE_DIRS "/usr/include")
#SET(OpenCV_LIB_DIR "/usr/lib")
#INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})



INCLUDE_DIRECTORIES(.)

add_library( # Sets the name of the library.
             CDlibFace

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             CDlibFace.cpp  CDlibFace.h)

target_link_libraries( # Specifies the target library.
                       CDlibFace

                       dlib)
