from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    pkg = get_package_share_directory('fast_livo')
    use_sim_time = LaunchConfiguration('use_sim_time')

    return LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value='true'),
        Node(
            package='fast_livo',
            executable='fastlivo_mapping',
            name='laserMapping',
            output='screen',
            parameters=[
                os.path.join(pkg, 'config', 'robot_room.yaml'),
                os.path.join(pkg, 'config', 'camera_robot_room.yaml'),
                {'use_sim_time': use_sim_time},
            ],
        ),
    ])
