'''
Arudino Serial Bridge Launch

Runs the serial bridge
'''
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    #Serial Bridge 
    my_rover_bringup = Node(
        package='my_rover_bringup',
        executable='ar_sr_br',
        output='screen'
    )
    
    # Create launch description and add actions
    ld = LaunchDescription()
    
    ld.add_action(my_rover_bringup)
    
    return ld