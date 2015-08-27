/*
 * Copyright 2015 Fadri Furrer, ASL, ETH Zurich, Switzerland
 * Copyright 2015 Michael Burri, ASL, ETH Zurich, Switzerland
 * Copyright 2015 Mina Kamel, ASL, ETH Zurich, Switzerland
 * Copyright 2015 Janosch Nikolic, ASL, ETH Zurich, Switzerland
 * Copyright 2015 Markus Achtelik, ASL, ETH Zurich, Switzerland
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef rotor_sim_joy_JOY_H_
#define rotor_sim_joy_JOY_H_

#include <geometry_msgs/PoseStamped.h>
#include <rotor_gazebo/RollPitchYawrateThrust.h>
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <relative_nav_msgs/Command.h>

struct Axes {
  int roll;
  int pitch;
  int thrust;
  int yaw;
  int roll_direction;
  int pitch_direction;
  int thrust_direction;
  int yaw_direction;
};

struct Max {
  double v_xy;
  double roll;
  double pitch;
  double rate_yaw;
  double thrust;
};

struct Button{
  int index;
  bool prev_value;
};

struct Buttons {
  Button fly;
};

class Joy {
  typedef sensor_msgs::Joy::_buttons_type ButtonType;

 private:
  ros::NodeHandle nh_;
  ros::Publisher ctrl_pub_;
  ros::Publisher command_pub_;
  ros::Subscriber joy_sub_;

  std::string namespace_;
  std::string command_topic_;

  Axes axes_;

  bool fly_mav_;

  rotor_gazebo::RollPitchYawrateThrust control_msg_;
  relative_nav_msgs::Command command_msg_;
  geometry_msgs::PoseStamped pose_;
  sensor_msgs::Joy current_joy_;

  Max max_;
  Buttons buttons_;

  double current_yaw_vel_;
  double v_yaw_step_;

  void StopMav();

  void JoyCallback(const sensor_msgs::JoyConstPtr& msg);
  void Publish();

 public:
  Joy();
};

#endif // rotor_sim_joy_JOY_H_
