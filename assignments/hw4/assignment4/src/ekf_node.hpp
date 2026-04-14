
#ifndef __EKF_NODE__
#define __EKF_NODE__
  
//#include <ros/ros.h>
//#include <tf/tf.h>
//#include <tf/transform_listener.h>
//#include <tf/transform_broadcaster.h>

#include "ekf.hpp"

#include <rclcpp/rclcpp.hpp>

// messages
#include <geometry_msgs/msg/twist.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>

#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

using namespace geometry_msgs::msg;
using namespace sensor_msgs::msg;
using namespace message_filters;

class EKFNode : public rclcpp::Node{

private:

  // shorthand typedefs
  typedef Subscriber<NavSatFix>    SubscribeGPS;
  typedef Subscriber<Imu>          SubscribeIMU;
  typedef sync_policies::ApproximateTime<NavSatFix,Imu> Policy;
  typedef message_filters::Synchronizer< EKFNode::Policy > Sync;
  
  SubscribeGPS    sub_gps;
  SubscribeIMU    sub_imu;
  Sync            sync;

  BFL::ColumnVector z;
  BFL::ColumnVector u;
  double period;
  
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_twist;
  rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr  pub_pose;

  EKF ekf;

  std::shared_ptr<rclcpp::TimerBase> timer;
  void callback_timer();
  
public:

  EKFNode();
  
  void callback_sensors( const NavSatFix& nsf, const Imu& imu );
  void callback_command( const Twist& vw );
  void update();

};

#endif
