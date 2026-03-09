#!/usr/bin/env python3
'''
Arduino Serial Bridge

Purujit Kantiya
MREN 203

- subs to /cmd_vel output from NAV2 and sends serial commands through UART SERIAL
- Publishes encoder data back to /odom
'''

import rclpy
from rclpy.node import Node
from tf2_ros import TransformBroadcaster
import serial
import time
import math

# INCLUDE MESSAGE TYPES
'''
message type for:
- Velocity from NAV2 /cmd_vel
- Encoder data to /odom node
'''
from geometry_msgs.msg import Twist, TransformStamped
from nav_msgs.msg import Odometry


#Definitions
SERIAL_PORT = '/dev/ttyACM0'
BAUD_RATE = 115200
TPR = 3000
RADIUS = .0625
LENGTH = 0.2775

class ar_sr_br(Node):

    def __init__(self):
        super().__init__('ar_sr_br')

        #Initialize serial communications
        self.ser = serial.Serial(SERIAL_PORT,BAUD_RATE, timeout=0.1)
        time.sleep(2.0)  # wait for Arduino to reset after serial connect
        self.get_logger().info(f'Connected to Arduino on {SERIAL_PORT}')

        #ROS Subscribers/Publishers
        self.cmd_sub_ = self.create_subscription(Twist, '/cmd_vel', self.cmd_vel_callback, 10)
        self.odom_pub = self.create_publisher(Odometry,'/odom',10)
        self.tf_broadcaster = TransformBroadcaster(self)

        # Odometry state
        self.x     = 0.0
        self.y     = 0.0
        self.theta = 0.0
        self.prev_left_ticks  = 0
        self.prev_right_ticks = 0
        self.last_time = self.get_clock().now()

        # Timer to read serial at 20Hz
        self.create_timer(0.05, self.read_serial)

    def cmd_vel_callback(self, msg):
        #linear + angular velocities (that matter)
        linear = msg.linear.x
        angular = msg.angular.z

        #send out serial command
        cmd = f"CMD {linear} {angular}\n"
        self.ser.write(cmd.encode())

    def read_serial(self):
        try:
            while self.ser.in_waiting:
                line = self.ser.readline().decode('utf-8').strip()
                if line.startswith("ENC"):
                    parts = line.split()
                    if len(parts) == 3:
                        left_ticks  = int(parts[1])
                        right_ticks = int(parts[2])
                        self.update_odometry(left_ticks, right_ticks)
                        self.get_logger().info(f'Left Ticks: {parts[1]} Right Ticks: {parts[2]}')
        except Exception as e:
            self.get_logger().warn(f'Serial read error: {e}')

    def update_odometry(self, left_ticks, right_ticks):
        now = self.get_clock().now()
        dt  = (now - self.last_time).nanoseconds / 1e9
        self.last_time = now

        # Ticks delta since last reading
        d_left  = (left_ticks  - self.prev_left_ticks)  * (2 * math.pi * RADIUS/TPR)
        d_right = (right_ticks - self.prev_right_ticks) * (2 * math.pi * RADIUS/TPR)
        self.prev_left_ticks  = left_ticks
        self.prev_right_ticks = right_ticks

        # Distance and heading change
        d_center = (d_left + d_right) / 2.0
        d_theta  = (d_right - d_left) / LENGTH

        # Update pose
        self.x     += d_center * math.cos(self.theta + d_theta / 2.0)
        self.y     += d_center * math.sin(self.theta + d_theta / 2.0)
        self.theta += d_theta

        # Velocities
        vx = d_center / dt if dt > 0 else 0.0
        wz = d_theta  / dt if dt > 0 else 0.0

        # Build and publish Odometry message
        odom = Odometry()
        odom.header.stamp    = now.to_msg()
        odom.header.frame_id = 'odom'
        odom.child_frame_id  = 'base_link'

        odom.pose.pose.position.x = self.x
        odom.pose.pose.position.y = self.y
        odom.pose.pose.orientation.z = math.sin(self.theta / 2.0)
        odom.pose.pose.orientation.w = math.cos(self.theta / 2.0)
        odom.twist.twist.linear.x  = vx
        odom.twist.twist.angular.z = wz

        self.odom_pub.publish(odom)

        # Broadcast odom -> base_link TF
        tf = TransformStamped()
        tf.header.stamp    = now.to_msg()
        tf.header.frame_id = 'odom'
        tf.child_frame_id  = 'base_link'
        tf.transform.translation.x = self.x
        tf.transform.translation.y = self.y
        tf.transform.rotation.z    = math.sin(self.theta / 2.0)
        tf.transform.rotation.w    = math.cos(self.theta / 2.0)
        self.tf_broadcaster.sendTransform(tf)



def main(args=None):
    rclpy.init(args=args)

    ar_sr_br_ = ar_sr_br()

    rclpy.spin(ar_sr_br_)

    ar_sr_br_.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()