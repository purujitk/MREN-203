import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    # 1. Get directories
    bringup_dir = get_package_share_directory('my_rover_bringup')
    nav2_bringup_dir = get_package_share_directory('nav2_bringup')

    # 2. Path to config file
    nav2_params_file = os.path.join(bringup_dir, 'config', 'nav2_params.yaml')

    # 3. Declare arguments
    use_sim_time = LaunchConfiguration('use_sim_time')

    declare_use_sim_time_cmd = DeclareLaunchArgument(
        'use_sim_time',
        default_value='false',
        description='Use simulation clock'
    )

    # 4. Use slam_launch.py instead of bringup_launch.py — no map required
    nav2_bringup_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(nav2_bringup_dir, 'launch', 'slam_launch.py')
        ),
        launch_arguments={
            'params_file': nav2_params_file,
            'use_sim_time': use_sim_time,
        }.items()
    )

    # 5. Create launch description
    ld = LaunchDescription()
    ld.add_action(declare_use_sim_time_cmd)
    ld.add_action(nav2_bringup_launch)
    return ld
