cmake_minimum_required(VERSION 3.16)
project(fast_livo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")

add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -pthread -fexceptions)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math")

add_definitions(-DROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")

include(ProcessorCount)
ProcessorCount(N)
if(N GREATER 4)
  add_definitions(-DMP_EN -DMP_PROC_NUM=4)
elseif(N GREATER 1)
  add_definitions(-DMP_EN -DMP_PROC_NUM=${N})
else()
  add_definitions(-DMP_PROC_NUM=1)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(livox_ros_driver2 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED COMPONENTS common io filters)
find_package(OpenCV REQUIRED)
find_package(OpenMP QUIET)
find_package(Boost REQUIRED COMPONENTS thread)

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)
add_definitions(${PCL_DEFINITIONS})

set(ROS_DEPS
  rclcpp
  std_msgs
  sensor_msgs
  geometry_msgs
  nav_msgs
  visualization_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
  pcl_conversions
  cv_bridge
  livox_ros_driver2
)

add_library(vio src/vio.cpp src/frame.cpp src/visual_point.cpp)
add_library(lio src/voxel_map.cpp)
add_library(pre src/preprocess.cpp)
add_library(imu_proc src/IMU_Processing.cpp)
add_library(laser_mapping src/LIVMapper.cpp)

foreach(target vio lio pre imu_proc laser_mapping)
  ament_target_dependencies(${target} ${ROS_DEPS})
  target_link_libraries(${target} ${PCL_LIBRARIES} ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})
  if(OpenMP_CXX_FOUND)
    target_link_libraries(${target} OpenMP::OpenMP_CXX)
  endif()
endforeach()

target_link_libraries(laser_mapping vio lio pre imu_proc)

add_executable(fastlivo_mapping src/main.cpp)
ament_target_dependencies(fastlivo_mapping ${ROS_DEPS})
target_link_libraries(fastlivo_mapping
  laser_mapping
  vio
  lio
  pre
  imu_proc
  ${PCL_LIBRARIES}
  ${OpenCV_LIBRARIES}
  ${Boost_LIBRARIES}
)
if(OpenMP_CXX_FOUND)
  target_link_libraries(fastlivo_mapping OpenMP::OpenMP_CXX)
endif()

install(TARGETS
  fastlivo_mapping
  laser_mapping
  vio
  lio
  pre
  imu_proc
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY config launch rviz_cfg
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
