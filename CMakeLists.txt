cmake_minimum_required(VERSION 3.0.2)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)
project(CameraVideoTopic)
rosbuild_init()
MESSAGE("Build type: " ${ROS_BUILD_TYPE})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -O3 -march=native")

# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    add_definitions(-DCOMPILEDWITHC11)
    message(STATUS "Using flag -std=c++11.")
elseif (COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    add_definitions(-DCOMPILEDWITHC0X)
    message(STATUS "Using flag -std=c++0x.")
else ()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif ()

set(OpenCV_DIR “/usr/local/share/OpenCV”)
find_package(OpenCV 3.4 REQUIRED)
if (NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV > 3.4 not found.")
endif ()
find_package(catkin REQUIRED COMPONENTS
        cv_bridge
        image_transport
        roscpp
        sensor_msgs
        std_msgs
        message_generation
        )
add_message_files(
        FILES
        NavSatFix.msg
)
generate_messages(
        DEPENDENCIES
        std_msgs
)
include_directories(
        ${catkin_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
)

rosbuild_add_executable(CameraPublish src/CameraPublish.cpp)
rosbuild_add_executable(ImgView src/ImgView.cpp)
rosbuild_add_executable(SubscribeTopic src/SubscribeTopic.cpp)

catkin_package(
        CATKIN_DEPENDS roscpp std_msgs message_runtime
)

target_link_libraries(CameraPublish ${catkin_LIBRARIES}
        /usr/local/lib/libopencv_imgproc.so.3.4
        /usr/local/lib/libopencv_core.so.3.4)
target_link_libraries(ImgView ${catkin_LIBRARIES}
        /usr/local/lib/libopencv_imgproc.so.3.4
        /usr/local/lib/libopencv_core.so.3.4)
target_link_libraries(SubscribeTopic ${catkin_LIBRARIES})