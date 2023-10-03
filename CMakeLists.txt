cmake_minimum_required(VERSION 3.0.2)
project(realtime_stitch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CUDA
set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
find_package(CUDA 11 REQUIRED)

# pugixml
find_package(pugixml REQUIRED)

#opencv
set(OpenCV_DIR /usr/local/lib/cmake/opencv4)
find_package(OpenCV 4.8.0 REQUIRED PATHS /home/sean/Documents/opencv480/build/)

# find_package(OpenCV 4 REQUIRED PATHS /path/to/your/opencv/installation NO_DEFAULT_PATH)

# TBB
find_package(TBB REQUIRED)

#Yaml
find_package(yaml-cpp REQUIRED)

# ROS dependencies
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  image_transport
  cv_bridge
)

catkin_package(
  CATKIN_DEPENDS 
  cv_bridge 
  image_transport 
)

# Include directories
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${catkin_INCLUDE_DIRS}
    include/${PROJECT_NAME}/
    # ${TBB_INCLUDE_DIRS}
)

# Header files
set(PROJECT_HEADERS
    include/inference.h
    include/imagecropper.h
    include/findfeature.h
    include/pairwisematches.h
    include/filterimage.h
    include/estimator.h
    include/adjusterimage.h
    include/focalsdecide.h
    include/warpingimage.h
    include/exposcompensation.h
    include/findseam.h
    include/imagescaler.h
    include/imageblender.h
    include/runyolo.h
    include/stitchingloop.h
)

# Source files
set(PROJECT_SOURCES
    src/imagecropper.cpp
    src/inference.cpp
    src/findfeature.cpp
    src/pairwisematches.cpp
    src/filterimage.cpp
    src/estimator.cpp
    src/adjusterimage.cpp
    src/focalsdecide.cpp
    src/warpingimage.cpp
    src/exposcompensation.cpp
    src/findseam.cpp
    src/imagescaler.cpp
    src/imageblender.cpp
    src/runyolo.cpp
    src/stitchingloop.cpp
)

# Source files for ROS node
set(ROS_NODE_SOURCES
    nodes/realtime_stitch_node.cpp
)

# Create the ROS node executable
add_executable(realtime_stitch_node nodes/realtime_stitch_node.cpp ${PROJECT_SOURCES})
target_link_libraries(realtime_stitch_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES} ${YAML_CPP_LIBRARIES} pugixml TBB::tbb)

add_executable(runyolo_node nodes/runyolo_node.cpp ${PROJECT_SOURCES})
target_link_libraries(runyolo_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES} ${YAML_CPP_LIBRARIES} pugixml TBB::tbb)