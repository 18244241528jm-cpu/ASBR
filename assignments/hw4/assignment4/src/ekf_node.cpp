#include "ekf_node.hpp"
#include "utilities.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

using namespace std::chrono_literals;

 // constructor
EKFNode::EKFNode() :
  Node("ekf"),
  sub_gps( this, "gps" ),
  sub_imu( this, "imu" ),
  sync( Policy(20), sub_gps, sub_imu ),
  z( 6 ),
  u(3),
  period(0.1){

  sync.registerCallback( &EKFNode::callback_sensors, this );
  
  // advertise our estimation
  pub_pose=create_publisher<PoseWithCovarianceStamped>("posterior",10);
  sub_twist=create_subscription<Twist>( "cmd_vel", 2,std::bind(&EKFNode::callback_command, this, std::placeholders::_1 ) );

  timer = create_wall_timer(100ms, std::bind(&EKFNode::callback_timer, this));

}

void EKFNode::callback_timer(){
  if( ekf.is_initialized() ){ 
    ekf.update( z, u ); 
    pub_pose->publish( ekf.get_posterior() );
  }  
}

// Sensor callback
// This only copies the measurement
void EKFNode::callback_sensors( const NavSatFix& nsf, const Imu& imu ){

  // combine the sensors
  z(1) = nsf.latitude;
  z(2) = nsf.longitude;
  z(3) = nsf.altitude;
  
  tf2::Quaternion q;
  tf2::fromMsg( imu.orientation, q );
  tf2::Matrix3x3 R( q );

  double r, p, y;
  rotm2rpy( R, r, p, y );

  z(4) = r;
  z(5) = p;
  z(6) = y;

}

// Velocity command callback
// This calls the EKF with the latest measurements
void EKFNode::callback_command( const Twist& vw ){
  u(1) = vw.linear.x;
  u(2) = vw.angular.z;
  u(3) = period;
  if(!ekf.is_initialized())
    {ekf.initialize();}
}


int main(int argc, char **argv){
  
  // Initialize ROS
  rclcpp::init( argc, argv );
  rclcpp::spin( std::make_shared<EKFNode>() );

  return 0;
}
