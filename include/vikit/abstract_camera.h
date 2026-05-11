#pragma once
#include <Eigen/Core>

namespace vk {
class AbstractCamera {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  AbstractCamera(int width, int height, double fx, double fy, double cx, double cy, double scale = 1.0)
      : width_(width), height_(height), fx_(fx), fy_(fy), cx_(cx), cy_(cy), scale_(scale) {}
  virtual ~AbstractCamera() = default;

  virtual Eigen::Vector2d world2cam(const Eigen::Vector3d &xyz) const {
    return Eigen::Vector2d(fx_ * xyz.x() / xyz.z() + cx_, fy_ * xyz.y() / xyz.z() + cy_);
  }
  virtual Eigen::Vector3d cam2world(double u, double v) const {
    return Eigen::Vector3d((u - cx_) / fx_, (v - cy_) / fy_, 1.0).normalized();
  }
  Eigen::Vector3d cam2world(const Eigen::Vector2d &px) const { return cam2world(px.x(), px.y()); }
  bool isInFrame(const Eigen::Vector2i &px, int border = 0) const {
    return px.x() >= border && px.y() >= border && px.x() < width_ - border && px.y() < height_ - border;
  }
  int width() const { return width_; }
  int height() const { return height_; }
  double fx() const { return fx_; }
  double fy() const { return fy_; }
  double cx() const { return cx_; }
  double cy() const { return cy_; }
  double scale() const { return scale_; }

protected:
  int width_;
  int height_;
  double fx_;
  double fy_;
  double cx_;
  double cy_;
  double scale_;
};
}  // namespace vk
