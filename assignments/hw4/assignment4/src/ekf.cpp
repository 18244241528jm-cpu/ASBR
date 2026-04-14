/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Wim Meeussen */

#include "ekf.hpp"
#include "ekf_models.hpp"
#include "utilities.h"

using namespace MatrixWrapper;
using namespace BFL;
using namespace tf2;
using namespace std;

// constructor
EKF::EKF():
  prior_( NULL ),
  filter_( NULL ),
  initialized( false ){


  // create system model
  ColumnVector sysNoise_Mu(6);  sysNoise_Mu = 0;
  SymmetricMatrix sysNoise_Cov(6); sysNoise_Cov = 0;
  
  //process_covariance( C );
  for( int r=1; r<=6; r++ )
    { sysNoise_Cov( r, r ) = 1.0; }
  
  Gaussian system_Uncertainty( sysNoise_Mu, sysNoise_Cov );
  sys_pdf_  = new SystemPDF( system_Uncertainty );
  sys_model_ = new AnalyticSystemModelGaussianUncertainty( sys_pdf_ );
  
  // create measurement model
  ColumnVector measNoise_Mu(6);  measNoise_Mu = 0;
  SymmetricMatrix measNoise_Cov(6);  measNoise_Cov = 0;
  
  //measurement_covariance( C );
  for( int r=1; r<=6; r++ )
    { measNoise_Cov( r, r ) = 1.0; }

  Gaussian measurement_Uncertainty( measNoise_Mu, measNoise_Cov );
  meas_pdf_   = new MeasurementPDF( measurement_Uncertainty );
  meas_model_ = new AnalyticMeasurementModelGaussianUncertainty( meas_pdf_ );

}

// destructor
EKF::~EKF(){
  if (filter_)      delete filter_;
  if (prior_)       delete prior_;
  if( sys_pdf_)     delete sys_pdf_;
  if( sys_model_)   delete sys_model_;
  if( meas_pdf_ )   delete meas_pdf_;
  if( meas_model_ ) delete meas_model_;
}

// initialize prior density of filter 
void EKF::initialize(){

  ColumnVector prior_Mu(6);
  prior_Mu = 0.0;
  prior_Mu(3) = 0.23;

  SymmetricMatrix prior_Cov(6);
  for (unsigned int i=1; i<=6; i++){
    for (unsigned int j=1; j<=6; j++){
      if (i==j) { prior_Cov(i,j) = pow( 3, 2 ); }
      else      { prior_Cov(i,j) = 0; }
    }
  }
  prior_Cov(4,4) = pow( 1, 2 ); 
  prior_Cov(5,5) = pow( 1, 2 ); 
  prior_Cov(6,6) = pow( 1, 2 ); 
  
  prior_  = new Gaussian( prior_Mu, prior_Cov );
  filter_ = new ExtendedKalmanFilter( prior_ );
  
  initialized = true;

}

// return the posterior estimate
geometry_msgs::msg::PoseWithCovarianceStamped EKF::get_posterior() const {

  ColumnVector mu = filter_->PostGet()->ExpectedValueGet();
  
  geometry_msgs::msg::PoseWithCovarianceStamped Rt;
  //Rt.header.stamp = now();
  Rt.header.frame_id = "map";
  Rt.pose.pose.position.x = mu(1);
  Rt.pose.pose.position.y = mu(2);
  Rt.pose.pose.position.z = mu(3);

  tf2::Matrix3x3 R = rpy2rotm( mu(4), mu(5), mu(6) );
  tf2::Quaternion q;
  R.getRotation(q);

  Rt.pose.pose.orientation.x = q.getX();
  Rt.pose.pose.orientation.y = q.getY();
  Rt.pose.pose.orientation.z = q.getZ();
  Rt.pose.pose.orientation.w = q.getW();

  SymmetricMatrix C = filter_->PostGet()->CovarianceGet();
  for(unsigned int i=0; i<6; i++)
    for(unsigned int j=0; j<6; j++)
      Rt.pose.covariance[6*i+j] = C(i+1,j+1);

  return Rt;

}

// update filter
bool EKF::update( const ColumnVector& z, const ColumnVector& u ){

  filter_->Update( sys_model_, u, meas_model_, z );

  return true;
}
