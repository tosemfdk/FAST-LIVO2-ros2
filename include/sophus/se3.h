#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace Sophus {
class SE3 {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  SE3() : R_(Eigen::Matrix3d::Identity()), t_(Eigen::Vector3d::Zero()) {}
  SE3(const Eigen::Matrix3d &R, const Eigen::Vector3d &t) : R_(R), t_(t) {}

  const Eigen::Matrix3d &rotation_matrix() const { return R_; }
  Eigen::Vector3d translation() const { return t_; }
  SE3 inverse() const { Eigen::Matrix3d Rt = R_.transpose(); return SE3(Rt, -(Rt * t_)); }
  SE3 operator*(const SE3 &other) const { return SE3(R_ * other.R_, R_ * other.t_ + t_); }
  Eigen::Vector3d operator*(const Eigen::Vector3d &p) const { return R_ * p + t_; }

private:
  Eigen::Matrix3d R_;
  Eigen::Vector3d t_;
};
}  // namespace Sophus
