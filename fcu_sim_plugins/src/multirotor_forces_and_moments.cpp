/*
 * Copyright 2016 James Jackson, Brigham Young University, Provo UT
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

#include "fcu_sim_plugins/multirotor_forces_and_moments.h"

namespace gazebo
{

MultiRotorForcesAndMoments::MultiRotorForcesAndMoments()
{

}


MultiRotorForcesAndMoments::~MultiRotorForcesAndMoments()
{
  event::Events::DisconnectWorldUpdateBegin(updateConnection_);
  if (nh_) {
    nh_->shutdown();
    delete nh_;
  }
}


void MultiRotorForcesAndMoments::SendForces()
{
  // apply the forces and torques to the joint
  // Gazebo is in NWU, while we calculate forces in NED, hence the negatives
  link_->AddRelativeForce(math::Vector3(actual_forces_.Fx, -actual_forces_.Fy, -actual_forces_.Fz));
  link_->AddRelativeTorque(math::Vector3(actual_forces_.l, -actual_forces_.m, -actual_forces_.n));
}


void MultiRotorForcesAndMoments::Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
{
  model_ = _model;
  world_ = model_->GetWorld();

  namespace_.clear();

  /*
   * Connect the Plugin to the Robot and Save pointers to the various elements in the simulation
   */
  if (_sdf->HasElement("namespace"))
    namespace_ = _sdf->GetElement("namespace")->Get<std::string>();
  else
    gzerr << "[multirotor_forces_and_moments] Please specify a namespace.\n";
  nh_ = new ros::NodeHandle(namespace_);

  if (_sdf->HasElement("linkName"))
    link_name_ = _sdf->GetElement("linkName")->Get<std::string>();
  else
    gzerr << "[multirotor_forces_and_moments] Please specify a linkName of the forces and moments plugin.\n";
  link_ = model_->GetLink(link_name_);
  if (link_ == NULL)
    gzthrow("[multirotor_forces_and_moments] Couldn't find specified link \"" << link_name_ << "\".");

  /* Load Params from Gazebo Server */
  getSdfParam<std::string>(_sdf, "windSpeedTopic", wind_speed_topic_, "wind");
  getSdfParam<std::string>(_sdf, "commandTopic", command_topic_, "command");
  getSdfParam<std::string>(_sdf, "attitudeTopic", attitude_topic_, "attitude");

  /* Load Params from ROS Server */
  mass_ = nh_->param<double>("mass", 3.856);
  linear_mu_ = nh_->param<double>("linear_mu", 0.1);
  angular_mu_ = nh_->param<double>("angular_mu", 0.5);

  // Drag Constant
  linear_mu_ = nh_->param<double>( "linear_mu", 0.8);
  angular_mu_ = nh_->param<double>( "angular_mu", 0.5);

  /* Ground Effect Coefficients */
  std::vector<double> ground_effect_list = {-55.3516, 181.8265, -203.9874, 85.3735, -7.6619};
  nh_->getParam("ground_effect", ground_effect_list);
  ground_effect_.a = ground_effect_list[0];
  ground_effect_.b = ground_effect_list[1];
  ground_effect_.c = ground_effect_list[2];
  ground_effect_.d = ground_effect_list[3];
  ground_effect_.e = ground_effect_list[4];

  // Build Actuators Container
  actuators_.l.max = nh_->param<double>("max_l", .2); // N-m
  actuators_.m.max = nh_->param<double>("max_m", .2); // N-m
  actuators_.n.max = nh_->param<double>("max_n", .2); // N-m
  actuators_.F.max = nh_->param<double>("max_F", 1.0); // N
  actuators_.l.tau_up = nh_->param<double>("tau_up_l", .25);
  actuators_.m.tau_up = nh_->param<double>("tau_up_m", .25);
  actuators_.n.tau_up = nh_->param<double>("tau_up_n", .25);
  actuators_.F.tau_up = nh_->param<double>("tau_up_F", 0.25);
  actuators_.l.tau_down = nh_->param<double>("tau_down_l", .25);
  actuators_.m.tau_down = nh_->param<double>("tau_down_m", .25);
  actuators_.n.tau_down = nh_->param<double>("tau_down_n", .25);
  actuators_.F.tau_down = nh_->param<double>("tau_down_F", 0.35);

  // Get PID Gains
  double rollP, rollI, rollD;
  double pitchP, pitchI, pitchD;
  double yawP, yawI, yawD;
  double altP, altI, altD;
  rollP = nh_->param<double>("roll_P", 0.1);
  rollI = nh_->param<double>("roll_I", 0.0);
  rollD = nh_->param<double>("roll_D", 0.0);
  pitchP = nh_->param<double>("pitch_P", 0.1);
  pitchI = nh_->param<double>("pitch_I", 0.0);
  pitchD = nh_->param<double>("pitch_D", 0.0);
  yawP = nh_->param<double>("yaw_P", 0.1);
  yawI = nh_->param<double>("yaw_I", 0.0);
  yawD = nh_->param<double>("yaw_D", 0.0);
  altP = nh_->param<double>("alt_P", 0.1);
  altI = nh_->param<double>("alt_I", 0.0);
  altD = nh_->param<double>("alt_D", 0.0);

  roll_controller_.setGains(rollP, rollI, rollD);
  pitch_controller_.setGains(pitchP, pitchI, pitchD);
  yaw_controller_.setGains(yawP, yawI, yawD);
  alt_controller_.setGains(altP, altI, altD);

  // Connect the update function to the simulation
  updateConnection_ = event::Events::ConnectWorldUpdateBegin(boost::bind(&MultiRotorForcesAndMoments::OnUpdate, this, _1));

  // Connect Subscribers
  command_sub_ = nh_->subscribe(command_topic_, 1, &MultiRotorForcesAndMoments::CommandCallback, this);
  wind_speed_sub_ = nh_->subscribe(wind_speed_topic_, 1, &MultiRotorForcesAndMoments::WindSpeedCallback, this);

  // Connect Publishers
  attitude_pub_ = nh_->advertise<rosflight_msgs::Attitude>(attitude_topic_, 1);

  // Initialize State
  this->Reset();
}

// This gets called by the world update event.
void MultiRotorForcesAndMoments::OnUpdate(const common::UpdateInfo& _info) {

  sampling_time_ = _info.simTime.Double() - prev_sim_time_;
  prev_sim_time_ = _info.simTime.Double();
  UpdateForcesAndMoments();
  SendForces();
}

void MultiRotorForcesAndMoments::WindSpeedCallback(const geometry_msgs::Vector3 &wind){
  W_wind_speed_.x = wind.x;
  W_wind_speed_.y = wind.y;
  W_wind_speed_.z = wind.z;
}

void MultiRotorForcesAndMoments::CommandCallback(const rosflight_msgs::Command msg)
{
  command_ = msg;
}

void MultiRotorForcesAndMoments::Reset()
{
  // Re-Initialize Memory Variables
  applied_forces_.Fx = 0;
  applied_forces_.Fy = 0;
  applied_forces_.Fz = 0;
  applied_forces_.l = 0;
  applied_forces_.m = 0;
  applied_forces_.n = 0;

  actual_forces_.Fx = 0;
  actual_forces_.Fy = 0;
  actual_forces_.Fz = 0;
  actual_forces_.l = 0;
  actual_forces_.m = 0;
  actual_forces_.n = 0;

  prev_sim_time_ = -1.0;
  sampling_time_ = -1.0;

  command_.mode = -1;

  // teleport the MAV to the initial position and reset it
  // link_->SetWorldPose(initial_pose_);
  // link_->ResetPhysicsStates();
}


void MultiRotorForcesAndMoments::UpdateForcesAndMoments()
{
  /* Get state information from Gazebo                          *
   * C denotes child frame, P parent frame, and W world frame.  *
   * Further C_pose_W_P denotes pose of P wrt. W expressed in C.*/
  // all coordinates are in standard aeronatical frame NED
  math::Pose W_pose_W_C = link_->GetWorldCoGPose();
  double pn = W_pose_W_C.pos.x;
  double pe = -W_pose_W_C.pos.y;
  double pd = -W_pose_W_C.pos.z;
  math::Vector3 euler_angles = W_pose_W_C.rot.GetAsEuler();
  double phi = euler_angles.x;
  double theta = -euler_angles.y;
  double psi = -euler_angles.z;
  math::Vector3 C_linear_velocity_W_C = link_->GetRelativeLinearVel();
  double u = C_linear_velocity_W_C.x;
  double v = -C_linear_velocity_W_C.y;
  double w = -C_linear_velocity_W_C.z;
  math::Vector3 C_angular_velocity_W_C = link_->GetRelativeAngularVel();
  double p = C_angular_velocity_W_C.x;
  double q = -C_angular_velocity_W_C.y;
  double r = -C_angular_velocity_W_C.z;

  // wind info is available in the wind_ struct
  // Rotate into body frame and relative velocity
  math::Vector3 C_wind_speed = W_pose_W_C.rot.RotateVector(W_wind_speed_);
  double ur = u - C_wind_speed.x;
  double vr = v - C_wind_speed.y;
  double wr = w - C_wind_speed.z;

  // calculate the appropriate control <- Depends on Control type (which block is being controlled)
  if (command_.mode < 0)
  {
    // We have not received a command yet.  This is not an error, but needs to be handled
  }
  else if (command_.mode == rosflight_msgs::Command::MODE_ROLLRATE_PITCHRATE_YAWRATE_THROTTLE)
  {
    desired_forces_.l = roll_controller_.computePID(command_.x, p, sampling_time_);
    desired_forces_.m = pitch_controller_.computePID(command_.y, q, sampling_time_);
    desired_forces_.n = yaw_controller_.computePID(command_.z, r, sampling_time_);
    desired_forces_.Fz = command_.F*actuators_.F.max; // this comes in normalized between 0 and 1
  }
  else if (command_.mode == rosflight_msgs::Command::MODE_ROLL_PITCH_YAWRATE_THROTTLE)
  {
    desired_forces_.l = roll_controller_.computePID(command_.x, phi, sampling_time_, p);
    desired_forces_.m = pitch_controller_.computePID(command_.y, theta, sampling_time_, q);
    desired_forces_.n = yaw_controller_.computePID(command_.z, r, sampling_time_);
    desired_forces_.Fz = command_.F*actuators_.F.max;
  }
  else if (command_.mode == rosflight_msgs::Command::MODE_ROLL_PITCH_YAWRATE_ALTITUDE)
  {
    desired_forces_.l = roll_controller_.computePID(command_.x, phi,  sampling_time_, p);
    desired_forces_.m = pitch_controller_.computePID(command_.y, theta, sampling_time_, q);
    desired_forces_.n = yaw_controller_.computePID(command_.z, r, sampling_time_);
    double pddot = -sin(theta)*u + sin(phi)*cos(theta)*v + cos(phi)*cos(theta)*w;
    double p1 = alt_controller_.computePID(command_.F, -pd, sampling_time_, -pddot);
    desired_forces_.Fz = p1  + (mass_*9.80665)/(cos(command_.x)*cos(command_.y));
  }

  // calculate the actual output force using low-pass-filters to introduce a first-order
  // approximation of delay in motor reponse
  // x(t+1) = Ce^(-t/tau)dt <- transfer to z-domain using backward differentiation

  // first get the appropriate tau for this situation
  double taul = (desired_forces_.l > applied_forces_.l ) ? actuators_.l.tau_up : actuators_.l.tau_down;
  double taum = (desired_forces_.m > applied_forces_.m ) ? actuators_.m.tau_up : actuators_.m.tau_down;
  double taun = (desired_forces_.n > applied_forces_.n ) ? actuators_.n.tau_up : actuators_.n.tau_down;
  double tauF = (desired_forces_.Fz > applied_forces_.Fz ) ? actuators_.F.tau_up : actuators_.F.tau_down;

  // calulate the alpha for the filter
  double alphal = sampling_time_/(taul + sampling_time_);
  double alpham = sampling_time_/(taum + sampling_time_);
  double alphan = sampling_time_/(taun + sampling_time_);
  double alphaF = sampling_time_/(tauF + sampling_time_);

  // Apply the discrete first-order filter
  applied_forces_.l = sat((1 - alphal)*applied_forces_.l + alphal *desired_forces_.l, actuators_.l.max, -1.0*actuators_.l.max);
  applied_forces_.m = sat((1 - alpham)*applied_forces_.m + alpham *desired_forces_.m, actuators_.m.max, -1.0*actuators_.m.max);
  applied_forces_.n = sat((1 - alphan)*applied_forces_.n + alphan *desired_forces_.n, actuators_.n.max, -1.0*actuators_.n.max);
  applied_forces_.Fz = sat((1 - alphaF)*applied_forces_.Fz + alphaF *desired_forces_.Fz, actuators_.F.max, 0.0);

  // calculate ground effect
  double z = -pd;
  double ground_effect = max(ground_effect_.a*z*z*z*z + ground_effect_.b*z*z*z + ground_effect_.c*z*z + ground_effect_.d*z + ground_effect_.e, 0);

  // Apply other forces (wind) <- follows "Quadrotors and Accelerometers - State Estimation With an Improved Dynamic Model"
  // By Rob Leishman et al. (Remember NED)
  actual_forces_.Fx = -1.0*linear_mu_*ur;
  actual_forces_.Fy = -1.0*linear_mu_*vr;
  actual_forces_.Fz = -1.0*linear_mu_*wr - applied_forces_.Fz - ground_effect;
  actual_forces_.l = -1.0*angular_mu_*p + applied_forces_.l;
  actual_forces_.m = -1.0*angular_mu_*q + applied_forces_.m;
  actual_forces_.n = -1.0*angular_mu_*r + applied_forces_.n;

  // publish attitude like ROSflight
  rosflight_msgs::Attitude attitude_msg;
  common::Time current_time  = world_->GetSimTime();
  attitude_msg.header.stamp.sec = current_time.sec;
  attitude_msg.header.stamp.nsec = current_time.nsec;
  attitude_msg.attitude.w =  W_pose_W_C.rot.w;
  attitude_msg.attitude.x =  W_pose_W_C.rot.x;
  attitude_msg.attitude.y = -W_pose_W_C.rot.y;
  attitude_msg.attitude.z = -W_pose_W_C.rot.z;

  attitude_msg.angular_velocity.x = p;
  attitude_msg.angular_velocity.y = q;
  attitude_msg.angular_velocity.z = r;

  attitude_pub_.publish(attitude_msg);
}

double MultiRotorForcesAndMoments::sat(double x, double max, double min)
{
  if(x > max)
    return max;
  else if(x < min)

    return min;
  else
    return x;
}

double MultiRotorForcesAndMoments::max(double x, double y)
{
  return (x > y) ? x : y;
}

GZ_REGISTER_MODEL_PLUGIN(MultiRotorForcesAndMoments);
}
