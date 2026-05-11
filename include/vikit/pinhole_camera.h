#pragma once
#include <array>
#include <cmath>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include "vikit/abstract_camera.h"

namespace vk {
class PinholeCamera : public AbstractCamera {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  PinholeCamera(int width, int height, double fx, double fy, double cx, double cy,
                double d0 = 0.0, double d1 = 0.0, double d2 = 0.0, double d3 = 0.0,
                double d4 = 0.0, double scale = 1.0)
      : AbstractCamera(width, height, fx, fy, cx, cy, scale), dist_{{d0, d1, d2, d3, d4}} {}

  void undistortImage(const cv::Mat &src, cv::Mat &dst) const {
    if (std::abs(dist_[0]) + std::abs(dist_[1]) + std::abs(dist_[2]) + std::abs(dist_[3]) + std::abs(dist_[4]) < 1e-12) {
      dst = src.clone();
      return;
    }
    cv::Mat K = (cv::Mat_<double>(3, 3) << fx_, 0.0, cx_, 0.0, fy_, cy_, 0.0, 0.0, 1.0);
    cv::Mat D = (cv::Mat_<double>(1, 5) << dist_[0], dist_[1], dist_[2], dist_[3], dist_[4]);
    cv::undistort(src, dst, K, D);
  }

private:
  std::array<double, 5> dist_;
};
}  // namespace vk
