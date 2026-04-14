
#ifndef __EKF_MODELS_HPP__
#define __EKF_MODELS_HPP__

#include <iostream>
#include <iomanip>

#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <sensor_msgs/msg/imu.hpp>


/**
   Message to send Roll Pitch Yaw orientation
*/
namespace sensor_msgs::msg{
  struct RPY{
    double roll;
    double pitch;
    double yaw;
  };
}

/**
   State of the robot
*/
struct State{

  enum { POS_X,      // X position
	 POS_Y,      // Y position
	 POS_Z,      // Z position
	 ROT_R,      // rotation about X
	 ROT_P,      // rotation about Y
	 ROT_Y };    // rotation about Z

  double x[6];      // the state vector

  friend std::ostream& operator << ( std::ostream& os, const State& s){

    os << std::setw(15) << "POS_X: " << s.x[POS_X]
       << std::setw(15) << "POS_Y: " << s.x[POS_Y]
       << std::setw(15) << "POS_Z: " << s.x[POS_Z] << std::endl
       << std::setw(15) << "ROT_R: " << s.x[ROT_R] 
       << std::setw(15) << "ROT_P: " << s.x[ROT_P]
       << std::setw(15) << "ROT_Y: " << s.x[ROT_Y] << std::endl;

    return os;

  }

};

/**
   TODO
   Fill in the value of the process covariance matrix. The rows/columns of C are
   in the following order [ POS_X POS_Y POS_Z ROT_R ROT_P ROT_Y ]. The 
   performance of your EKF will vary based on the values you provide here.
   \param[out] WMWt Covariance matrix of the system.
   \param state_in    The current state estimate
   \param v           The input linear velocity
   \param w           The input angular velocity
   \param dt          Delta time
*/
void sys_evaluate_WMWt( double WMWt[6][6], const State& state, double v, double w, double dt );

/**
   TODO
   Fill in the value of the measurement covariance matrix. The rows/columns of C
   are in the following order [POS_X POS_Y POS_Z ROT_R ROT_P ROT_Y ]. The 
   performance of your EKF will vary based on the values you provide here.
   \param[out] R Covariance matrix of the sensors.
   \param state_in    The current state estimate
*/
void meas_evaluate_R( double R[6][6], const State& state );

/**
   TODO
   Evaluate the system function.
   This function returns the prediction of the next state based on the 
   current state estimate and the commmand input (linear/angular velocities).
   \param state_in    The current state estimate
   \param v           The input linear velocity
   \param w           The input angular velocity
   \param dt          Delta time
*/
State sys_evaluate_f( const State& state_in, double v, double w, double dt );

/** 
   TODO
   Evaluate the system Jacobian.
   This function evaluates the Jacobian of the system functions g (see 
   sys_evaluate_g). The entry G[i][j] represents ( d g_i / d s_j )
   \param[out] G      The 6x6 Jacobian of the function g
   \param state       The state of the robot
   \param v           The input linear velocity
   \param w           The input angular velocity
   \param dt          Delta time
*/
void sys_evaluate_A( double G[6][6], const State& state, double v, double w, double dt );


/**
   TODO
   Evaluate the GPS observation function.
   This function returns the expected satellite fix given the state of the robot
   \param state_in The state estimate
   \return         A satellite navigation fix (only the latitute, longitude
                   and altitude members are used)
*/
sensor_msgs::msg::NavSatFix meas_evaluate_gps( const State& state );

/**
   Evaluate the IMU observation function.
   This function computes the expected imu orientation given the state of the 
   robot.
   \param state_in The current state estimate
   \return         A inertial navigation unit measurement (only the orientation
                   member is used).
*/
sensor_msgs::msg::RPY meas_evaluate_imu( const State& state );

/** 
    Observation Jacobian of the GPS
    This function returns the 3x3 observation Jacobian of the GPS. Essentially,
    this is the Jacobian of your meas_evaluate_gps function.
    \param[out] Hgps The 3x3 GPS Jacobian.
    \param[in]  state The state of the robot
*/
void meas_evaluate_Hgps( double Hgps[3][3], const State& state );

/** 
    Observation Jacobian of the IMU
    This function returns the 3x3 observation Jacobian of the IMU. Essentially,
    this is the Jacobian of your meas_evaluate_imu function.
    \param[out] Himu The 3x3 IMU Jacobian.
    \param[in]  state The state of the robot
*/
void meas_evaluate_Himu( double Himu[3][3], const State& state );

#endif
