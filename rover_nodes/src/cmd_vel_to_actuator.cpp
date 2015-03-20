/*
  ROS teleop node for the pixhawk autopilot

  subscribes to joy topic and sends AcutatorControl msg to pixhawk via mavros
  
  Copyright 2015 Marcel Stüttgen <stuettgen@fh-aachen.de>
*/

#include <ros/ros.h>
#include <mavros/ActuatorControl.h>
#include <geometry_msgs/Twist.h>
#include <string>

class CmdVelToActuator {
 public:
  //constructor
  CmdVelToActuator() :
      yaw_(0.0),
      throttle_(0.0)
  {
    nh_.param<std::string>("cmd_vel_topic", cmd_vel_topic_, "/cmd_vel");
    nh_.param<std::string>("actuator_controls_topic", acutator_controls_topic_, "/mavros/actuator_controls");

    cmd_vel_sub_ = nh_.subscribe(cmd_vel_topic_, 1, cmd_vel_callback);
    actuator_control_pub_ = nh_.advertise<mavros::ActuatorControl>(acutator_controls_topic_, 1);
  }


  void cmd_vel_callback(const geometry_msgs::Twist& vel_cmd)
  {
     ROS_INFO("I heard: [%s]", vel_cmd.linear.y);
     cout << "Twist Received " << endl;
     throttle_ = vel_cmd.linear.x;
     yaw_ = vel_cmd.angular.z;
     mavros::ActuatorControl a_c_msg;
     a_c_msg.header.stamp = ros::Time::now();
     a_c_msg.group_mix = 0;
     a_c_msg.controls[0] = 0.0;
     a_c_msg.controls[1] = 0.0;
     a_c_msg.controls[2] = yaw_;
     a_c_msg.controls[3] = throttle_;
     a_c_msg.controls[4] = 0.0;
     a_c_msg.controls[5] = 0.0;
     a_c_msg.controls[6] = 0.0;
     a_c_msg.controls[7] = 0.0;
     actuator_control_pub_.publish(a_c_msg);
  }
  
 private:
  ros::NodeHandle nh_;
  ros::Subscriber cmd_vel_sub_;
  ros::Publisher actuator_control_pub_;
  std::string cmd_vel_topic_;
  std::string acutator_controls_topic_;
  double yaw_, throttle_;
};



int main(int argc, char *argv[])
{
    //Initiate ROS
    ros::init(argc, argv, "pixhawk_teleop");
    ros::Rate r(10);

    //Create an object of class that will take care of everything
    CmdVelToActuator p;
    while(ros::ok()) {
      //need to publish frequently because pixhawk will go into failsafe mode else
      ros::spin();
      r.sleep();
    }
  return 0;
}
