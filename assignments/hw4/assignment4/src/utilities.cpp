#include <math.h>
#include "utilities.h"

/** Convert a rotation matrix to Roll Pitch Yaw angles.
    according to RPY = Rx(R) * Ry(P) * Rz(Y)
*/
void rotm2rpy( const tf2::Matrix3x3& RPY, double& R, double& P, double& Y ){

  // If singular (just in case)
  if( fabs(RPY[2][2]) < 1e-9 && fabs(RPY[1][2]) < 1e-9 ){
    R = 0.0;
    P = atan2( RPY[0][2], RPY[2][2] );
    Y = atan2( RPY[1][0], RPY[1][1]);
  }

  // otherwise. This should be used most of the time
  else{
    R = atan2( -RPY[1][2], RPY[2][2] );
    double sr = sin( R );
    double cr = cos( R );
    P = atan2(  RPY[0][2], cr*RPY[2][2] - sr*RPY[1][2] );
    Y = atan2( -RPY[0][1], RPY[0][0] );
  }

}

/** Convert a Roll Pitch Yaw angles to a rotation matrix
    according to RPY = Rx(R) * Ry(P) * Rz(Y)
*/
tf2::Matrix3x3 rpy2rotm( double R, double P, double Y ){

  tf2::Matrix3x3 Rx, Ry, Rz;
  Rx.setRPY( R, 0.0, 0.0 );
  Ry.setRPY( 0.0, P, 0.0 );
  Rz.setRPY( 0.0, 0.0, Y );

  return Rx * Ry * Rz;
}

