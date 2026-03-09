'''
Lidar Launch

Runs node from rplidar library
'''
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():

    rplidar_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('rplidar_ros'),
                'launch',
                'view_rplidar_a1_launch.py'
            )
        )
    )

    return LaunchDescription([
        rplidar_launch
    ])