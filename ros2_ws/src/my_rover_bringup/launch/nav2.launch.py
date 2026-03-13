import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    bringup_dir = get_package_share_directory('my_rover_bringup')
    nav2_bringup_dir = get_package_share_directory('nav2_bringup')

    nav2_params_file = os.path.join(bringup_dir, 'config', 'nav2_params.yaml')

    use_sim_time = LaunchConfiguration('use_sim_time')
    declare_use_sim_time_cmd = DeclareLaunchArgument(
        'use_sim_time',
        default_value='false',
        description='Use simulation clock'
    )

    # Use navigation_launch.py NOT slam_launch.py
    # Your main bringup already runs SLAM toolbox
    nav2_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(nav2_bringup_dir, 'launch', 'navigation_launch.py')
        ),
        launch_arguments={
            'params_file': nav2_params_file,
            'use_sim_time': use_sim_time,
        }.items()
    )

    # Frontier exploration node
    explore_node = Node(
        package='explore_lite',
        executable='explore',
        name='explore',
        output='screen',
        parameters=[{
            'robot_base_frame': 'base_footprint',
            'costmap_topic': '/map',
            'costmap_updates_topic': '/map_updates',
            'visualize': True,
            'planner_frequency': 0.33,
            'progress_timeout': 30.0,
            'potential_scale': 3.0,
            'gain_scale': 1.0,
            'min_frontier_size': 0.75,
            'use_sim_time': use_sim_time,
        }]
    )

    ld = LaunchDescription()
    ld.add_action(declare_use_sim_time_cmd)
    ld.add_action(nav2_launch)
    ld.add_action(TimerAction(period=8.0, actions=[explore_node]))
    return ld
