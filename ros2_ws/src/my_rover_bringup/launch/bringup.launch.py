'''
bring up node to launch full autnomy stack

Launches tehse in order:
    1. State publisher
    2. Lidar
    3. serial bridge
    4. slam
    5. Nav2

'''

import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource


def generate_launch_description():
    bringup_dir = get_package_share_directory('my_rover_bringup')
    
    # Include each sub-launch
    robot_state_pub_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(bringup_dir, 'launch', 'robot_state_publisher.launch.py')
        )
    )
    
    lidar_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(bringup_dir, 'launch', 'lidar.launch.py')
        )
    )
    
    arduino_bridge_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(bringup_dir, 'launch', 'arduino_bridge.launch.py')
        )
    )
    
    slam_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(bringup_dir, 'launch', 'slam.launch.py')
        )
    )
    
    nav2_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(bringup_dir, 'launch', 'nav2.launch.py')
        )
    )
    
    # Combine them
    ld = LaunchDescription()
    ld.add_action(robot_state_pub_launch)
    ld.add_action(lidar_launch)
    ld.add_action(arduino_bridge_launch)
    ld.add_action(slam_launch)
    ld.add_action(nav2_launch)
    
    return ld