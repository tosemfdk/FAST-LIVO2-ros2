# FAST-LIVO2 ROS2 Jazzy Port

FAST-LIVO2를 **ROS2 Jazzy**에서 바로 실행할 수 있게 포팅한 버전입니다. 현재 설정은 Livox ROS2 bag replay 기준이며, `/livox/lidar`, `/livox/imu`, `/camera/image_raw`를 구독해서 odometry, path, current scan, colored visual map을 publish합니다.

원본 프로젝트: [hku-mars/FAST-LIVO2](https://github.com/hku-mars/FAST-LIVO2)

## 현재 포팅 상태

- ROS1 `catkin/roscpp` 기반 코드를 ROS2 `ament_cmake/rclcpp` 기반으로 변경
- 패키지명: `fast_livo`
- 실행 노드: `fastlivo_mapping`
- 기본 런치: `launch/mapping_robot_room.launch.py`
- 기본 설정:
  - `config/robot_room.yaml`
  - `config/camera_robot_room.yaml`
  - `config/qos_overrides_robot_room.yaml`
- Livox bag replay용 호환 메시지 패키지 포함:
  - `livox_ros_driver2/msg/CustomMsg.msg`
  - `livox_ros_driver2/msg/CustomPoint.msg`
- RViz에서 보기 좋은 주요 출력 토픽만 유지:
  - `/cloud_registered` — 현재 scan
  - `/cloud_visual_map` — 누적 colored visual map
  - `/aft_mapped_to_init` — odometry
  - `/path` — trajectory
  - `/rgb_img` — debug image

## 환경

검증 기준 환경:

- Ubuntu 24.04
- ROS2 Jazzy
- PCL / Eigen / OpenCV / Boost

필요 패키지 예시:

```bash
sudo apt update
sudo apt install -y \
  python3-colcon-common-extensions \
  ros-jazzy-desktop \
  ros-jazzy-pcl-conversions \
  ros-jazzy-cv-bridge \
  ros-jazzy-tf2-geometry-msgs \
  libpcl-dev libeigen3-dev libopencv-dev libboost-thread-dev
```

## 빌드

이 repo root에서 바로 빌드할 수 있습니다.

```bash
cd /home/loe/workspace/github/FAST-LIVO2-ros2

source /opt/ros/jazzy/setup.bash
colcon build --symlink-install --packages-up-to fast_livo
source install/setup.bash
```

일반적인 ROS2 workspace 형태로 쓰고 싶으면 repo를 `~/ros2_ws/src/FAST-LIVO2-ros2`에 두고 `~/ros2_ws`에서 같은 `colcon build`를 실행하면 됩니다.

## 실행: FAST-LIVO2 노드

터미널 1:

```bash
cd /home/loe/workspace/github/FAST-LIVO2-ros2

source /opt/ros/jazzy/setup.bash
source install/setup.bash

ros2 launch fast_livo mapping_robot_room.launch.py use_sim_time:=true
```

`mapping_robot_room.launch.py`는 다음 설정을 로드합니다.

- `config/robot_room.yaml`
- `config/camera_robot_room.yaml`
- `use_sim_time:=true`

## 실행: rosbag replay

터미널 2:

```bash
cd /home/loe/workspace/github/FAST-LIVO2-ros2

source /opt/ros/jazzy/setup.bash
source install/setup.bash

ros2 bag play /home/loe/workspace/data/5_9_robot_room_cyclone \
  --clock \
  --qos-profile-overrides-path config/qos_overrides_robot_room.yaml
```

중요: bag 안의 `/livox/lidar` 타입이 `livox_ros_driver2/msg/CustomMsg`이므로, **bag play 터미널에서도 반드시 `source install/setup.bash`를 해야 합니다.** 안 하면 `package 'livox_ros_driver2' not found` 에러가 납니다.

## RViz로 보기

터미널 3:

```bash
cd /home/loe/workspace/github/FAST-LIVO2-ros2

source /opt/ros/jazzy/setup.bash
source install/setup.bash

rviz2 -d rviz_cfg/fast_livo2_ros2_minimal.rviz
```

RViz에서 직접 설정할 경우:

- Fixed Frame: `camera_init`
- PointCloud2:
  - `/cloud_registered`
  - `/cloud_visual_map`
- Odometry: `/aft_mapped_to_init`
- Path: `/path`
- Image: `/rgb_img` 선택 사항

## 입력 토픽

기본 `robot_room` 설정은 아래 bag 토픽을 사용합니다.

```text
/livox/lidar       livox_ros_driver2/msg/CustomMsg
/livox/imu         sensor_msgs/msg/Imu
/camera/image_raw  sensor_msgs/msg/Image
```

토픽명이 다르면 `config/robot_room.yaml`의 아래 항목을 수정하세요.

```yaml
common:
  img_topic: "/camera/image_raw"
  lid_topic: "/livox/lidar"
  imu_topic: "/livox/imu"
```

## 결과 저장 설정

PCD 저장은 기본적으로 꺼져 있습니다.

```yaml
pcd_save:
  pcd_save_en: false
  type: 0
  filter_size_pcd: 0.15
  interval: 2
```

저장이 필요하면 `config/robot_room.yaml`에서 `pcd_save_en: true`로 바꾼 뒤 다시 실행하세요.

## 자주 나는 문제

### `package 'livox_ros_driver2' not found`

bag play 또는 launch를 실행하는 터미널에서 이 repo의 install setup을 source하지 않은 상태입니다.

```bash
source /home/loe/workspace/github/FAST-LIVO2-ros2/install/setup.bash
```

### RViz에 아무것도 안 보임

1. Fixed Frame이 `camera_init`인지 확인
2. bag을 `--clock`으로 재생했는지 확인
3. FAST-LIVO2 launch가 `use_sim_time:=true`인지 확인
4. `/cloud_registered`, `/cloud_visual_map`, `/aft_mapped_to_init`, `/path` 토픽이 publish되는지 확인

```bash
ros2 topic list | grep -E 'cloud_registered|cloud_visual_map|aft_mapped_to_init|path'
```

### rosbag QoS 때문에 토픽을 못 받음

bag replay에 QoS override 파일을 같이 넣으세요.

```bash
ros2 bag play /path/to/bag --clock \
  --qos-profile-overrides-path config/qos_overrides_robot_room.yaml
```

## 원본 FAST-LIVO2 자료

- Paper: [FAST-LIVO2: Fast, Direct LiDAR-Inertial-Visual Odometry](https://arxiv.org/pdf/2408.14035)
- FAST-LIVO2 Dataset: [FAST-LIVO2-Dataset](https://connecthkuhk-my.sharepoint.com/:f:/g/personal/zhengcr_connect_hku_hk/ErdFNQtjMxZOorYKDTtK4ugBkogXfq1OfDm90GECouuIQA?e=KngY9Z)
- Calibration toolkit: [FAST-Calib](https://github.com/hku-mars/FAST-Calib)

## License

이 저장소는 원본 FAST-LIVO2 코드 기반의 ROS2 포팅입니다. 원본 코드와 포함된 서드파티 구성요소의 라이선스를 함께 확인하세요.
