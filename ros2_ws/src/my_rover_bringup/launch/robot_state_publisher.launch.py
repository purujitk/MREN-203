'''
State publiser launch file

starts transform broadcasts
'''
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, Command
from launch_ros.actions import Node


def generate_launch_description():
    # Get the package directory
    bringup_dir = get_package_share_directory('my_rover_bringup')
    description_dir = get_package_share_directory('my_rover_description')
    
    # Path to URDF file
    urdf_file = os.path.join(description_dir, 'urdf', 'my_rover.urdf.xacro')
    
    
    # Process URDF with xacro
    robot_description_content = Command(
        ['xacro ', urdf_file]
    )
    
    # Robot State Publisher node
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[
            {
                'robot_description': robot_description_content
            }
        ]
    )
    
    # Create launch description and add actions
    ld = LaunchDescription()
    
    ld.add_action(robot_state_publisher_node)
    
    return ld