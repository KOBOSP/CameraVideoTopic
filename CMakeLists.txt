cmake_minimum_required(VERSION 2.8.3)
project(CameraVideoTopic)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")


find_package(OpenCV 4 REQUIRED)
if (NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV 4 not found.")
endif ()


find_package(catkin REQUIRED COMPONENTS
        cv_bridge
        image_transport
        roscpp
        sensor_msgs
        std_msgs
        message_generation
        serial
        )
add_message_files(
        DIRECTORY msg
        FILES NavSatFix.msg
)
generate_messages(
        DEPENDENCIES
        std_msgs
)
catkin_package(INCLUDE_DIRS include
        LIBRARIES ch347 rt
        CATKIN_DEPENDS roscpp std_msgs
        DEPENDS ch347 rt
        )
include_directories(
        ${catkin_INCLUDE_DIRS} include
)
link_directories(
        ${catkin_LIB_DIRS} lib
)
message(STATUS "-------------catkin_INCLUDE_DIRS:${catkin_INCLUDE_DIRS},,catkin_LIB_DIRS:${catkin_LIB_DIRS}.")
set(SourceFile
        /home/kobosp/ublox_driver/catkin_ws/src/CameraVideoTopic/ch341_lib.h
        /home/kobosp/ublox_driver/catkin_ws/src/CameraVideoTopic/ch347_lib.h)

add_executable(PubLeft src/PubLeft.cpp)
add_executable(PubRight src/PubRight.cpp)
add_executable(PubPPS src/PubPPS.cpp)
add_executable(ImgView src/ImgView.cpp)
add_executable(PubIMU src/PubIMU.cpp)
add_executable(SubscribeTopic src/SubscribeTopic.cpp)

target_link_libraries(PubLeft ${catkin_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries(PubRight ${catkin_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries(PubPPS ${catkin_LIBRARIES} ch347 rt)
target_link_libraries(ImgView ${catkin_LIBRARIES} ${OpenCV_LIBS})
target_link_libraries(PubIMU ${catkin_LIBRARIES})
target_link_libraries(SubscribeTopic ${catkin_LIBRARIES} ${OpenCV_LIBS})