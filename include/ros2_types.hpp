#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <builtin_interfaces/msg/time.hpp>
#include <std_msgs/msg/header.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/path.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <livox_ros_driver2/msg/custom_msg.hpp>

using ImuMsg = sensor_msgs::msg::Imu;
using ImuConstPtr = sensor_msgs::msg::Imu::ConstSharedPtr;
using ImageMsg = sensor_msgs::msg::Image;
using ImageConstPtr = sensor_msgs::msg::Image::ConstSharedPtr;
using PointCloud2Msg = sensor_msgs::msg::PointCloud2;
using PointCloud2ConstPtr = sensor_msgs::msg::PointCloud2::ConstSharedPtr;
using LivoxCustomMsg = livox_ros_driver2::msg::CustomMsg;
using LivoxCustomConstPtr = livox_ros_driver2::msg::CustomMsg::ConstSharedPtr;

inline double stampToSec(const builtin_interfaces::msg::Time &stamp) {
  return rclcpp::Time(stamp).seconds();
}

inline builtin_interfaces::msg::Time secToStamp(const double seconds) {
  rclcpp::Time t(static_cast<int64_t>(seconds * 1000000000.0));
  return t;
}

inline std::string ros1ParamNameToRos2(std::string name) {
  std::replace(name.begin(), name.end(), '/', '.');
  return name;
}

template <typename T>
T declareAndGetParam(rclcpp::Node &node, const std::string &ros1_name, const T &default_value) {
  const auto name = ros1ParamNameToRos2(ros1_name);
  if (!node.has_parameter(name)) {
    node.declare_parameter<T>(name, default_value);
  }
  return node.get_parameter(name).get_value<T>();
}


template <>
inline std::vector<int> declareAndGetParam<std::vector<int>>(rclcpp::Node &node, const std::string &ros1_name, const std::vector<int> &default_value) {
  const auto name = ros1ParamNameToRos2(ros1_name);
  std::vector<int64_t> default_i64(default_value.begin(), default_value.end());
  if (!node.has_parameter(name)) {
    node.declare_parameter<std::vector<int64_t>>(name, default_i64);
  }
  const auto values = node.get_parameter(name).as_integer_array();
  return std::vector<int>(values.begin(), values.end());
}

inline rclcpp::Logger fastLivoLogger() { return rclcpp::get_logger("fast_livo"); }
