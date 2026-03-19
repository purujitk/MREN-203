import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/robot21/MREN-203/ros2_ws/install/my_rover_navigation'
