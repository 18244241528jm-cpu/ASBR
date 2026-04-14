
#include "ekf_models.hpp"
#include "utilities.h"
#include <cmath>

// ---------------------------------------------------------------------------
// GPS reference coordinates from assignment4.sdf (Gazebo spherical_coordinates)
// World frame orientation: ENU  →  X = East, Y = North, Z = Up
// ---------------------------------------------------------------------------
static const double GPS_LAT0 = -42.986687;   // degrees  (origin latitude)
static const double GPS_LON0 = -23.202501;   // degrees  (origin longitude)
static const double GPS_ALT0 =  18.123;      // meters   (origin elevation)

// Flat-earth scale factors at the reference latitude
// 1 degree latitude  ≈ 111 320 m  (N/S direction = world Y in ENU)
// 1 degree longitude ≈ 111 320 * cos(lat) m  (E/W direction = world X in ENU)
static const double M_PER_DEG_LAT = 111320.0;
static const double M_PER_DEG_LON = 111320.0 * std::cos( GPS_LAT0 * 3.14159265358979323846 / 180.0 );

/**
   Fill in the value of the process covariance matrix.
   Rows/columns order: [ POS_X POS_Y POS_Z ROT_R ROT_P ROT_Y ]

   Design choices
   ──────────────
   • Position XY noise scales with |v|·dt   – faster motion → more integration error.
   • Position Z  noise is a constant base    – terrain vertical changes are not commanded.
   • Roll/pitch   noise is a constant base   – driven by terrain contact, not by commands.
   • Yaw          noise scales with |w|·dt   – turning faster → more heading error.
   A small epsilon prevents a zero-diagonal covariance at rest.
*/
void sys_evaluate_WMWt( double WMWt[6][6], const State& state, double v, double w, double dt ){

  for( int r=0; r<6; r++ )
    for( int c=0; c<6; c++ )
      WMWt[r][c] = 0.0;

  const double av  = std::fabs(v);
  const double aw  = std::fabs(w);
  const double eps = 1e-6;          // floor to keep the matrix positive-definite

  // Tunable scale factors (adjust during Expert-challenge tuning)
  const double q_xy  = 0.10;        // positional error fraction  [dimensionless]
  const double q_z   = 0.05;        // vertical noise amplitude    [m/s]
  const double q_rp  = 0.05;        // roll/pitch noise rate       [rad/s]
  const double q_yaw = 0.10;        // yaw error fraction          [dimensionless]

  double s_xy  = q_xy  * av * dt;
  double s_z   = q_z        * dt;
  double s_rp  = q_rp       * dt;
  double s_yaw = q_yaw * aw * dt;

  WMWt[State::POS_X][State::POS_X] = s_xy  * s_xy  + eps;
  WMWt[State::POS_Y][State::POS_Y] = s_xy  * s_xy  + eps;
  WMWt[State::POS_Z][State::POS_Z] = s_z   * s_z   + eps;
  WMWt[State::ROT_R][State::ROT_R] = s_rp  * s_rp  + eps;
  WMWt[State::ROT_P][State::ROT_P] = s_rp  * s_rp  + eps;
  WMWt[State::ROT_Y][State::ROT_Y] = s_yaw * s_yaw + eps;
}

/**
   Fill in the measurement covariance matrix R.
   Rows/columns order: [ lat  lon  alt  roll  pitch  yaw ]

   GPS:  ~5 m horizontal, ~10 m vertical accuracy.
         Noise is expressed in the same units as the measurement (degrees for
         lat/lon, metres for altitude), using the flat-earth scale factors.
   IMU:  ~0.01 rad roll/pitch, ~0.02 rad yaw accuracy.
*/
void meas_evaluate_R( double R[6][6], const State& state ){

  for( int r=0; r<6; r++ )
    for( int c=0; c<6; c++ )
      R[r][c] = 0.0;

  // GPS position noise converted to degree²
  const double sigma_h   = 5.0;    // m – horizontal (lat and lon)
  const double sigma_alt = 10.0;   // m – vertical

  const double sigma_lat = sigma_h / M_PER_DEG_LAT;
  const double sigma_lon = sigma_h / M_PER_DEG_LON;

  R[0][0] = sigma_lat * sigma_lat;
  R[1][1] = sigma_lon * sigma_lon;
  R[2][2] = sigma_alt * sigma_alt;

  // IMU orientation noise
  const double sigma_rp  = 0.01;   // rad
  const double sigma_yaw = 0.02;   // rad

  R[3][3] = sigma_rp  * sigma_rp;
  R[4][4] = sigma_rp  * sigma_rp;
  R[5][5] = sigma_yaw * sigma_yaw;
}

/**
   Evaluate the system (process) function  s_{t+1} = f(s_t, u, dt).

   Kinematics for a wheeled robot in 3-D with rotation R = Rx(r)·Ry(p)·Rz(ψ):

     Body linear  velocity:  [v, 0, 0]ᵀ  →  world frame via R
     Body angular velocity:  [0, 0, ω]ᵀ  →  Euler rates via kinematic inversion
       (for R = Rx·Ry·Rz with body-Z angular input: ṙ = 0, ṗ = 0, ψ̇ = ω)

   Euler integration with step dt.
*/
State sys_evaluate_f( const State& state_in, double v, double w, double dt ){

  State state_out;

  const double x   = state_in.x[State::POS_X];
  const double y   = state_in.x[State::POS_Y];
  const double z   = state_in.x[State::POS_Z];
  const double r   = state_in.x[State::ROT_R];
  const double p   = state_in.x[State::ROT_P];
  const double psi = state_in.x[State::ROT_Y];

  const double cr = std::cos(r),   sr = std::sin(r);
  const double cp = std::cos(p),   sp = std::sin(p);
  const double cy = std::cos(psi), sy = std::sin(psi);

  // World-frame velocity  =  R · [v, 0, 0]ᵀ  (first column of R = Rx·Ry·Rz)
  const double xdot =  v * cp * cy;
  const double ydot =  v * ( cr*sy + sr*sp*cy );
  const double zdot =  v * ( sr*sy - cr*sp*cy );

  // Angular rate: body [0,0,ω]ᵀ  →  Euler rates  →  ṙ=0, ṗ=0, ψ̇=ω
  const double psidot = w;

  state_out.x[State::POS_X] = x   + dt * xdot;
  state_out.x[State::POS_Y] = y   + dt * ydot;
  state_out.x[State::POS_Z] = z   + dt * zdot;
  state_out.x[State::ROT_R] = r;                  // rdot = 0
  state_out.x[State::ROT_P] = p;                  // pdot = 0
  state_out.x[State::ROT_Y] = psi + dt * psidot;

  return state_out;
}

/**
   Evaluate the 6×6 system Jacobian  A = ∂f/∂s.

   Non-trivial partial derivatives (all others are either 0 or 1 on diagonal):

   f₀ = x + dt·v·cos(p)·cos(ψ)
     ∂f₀/∂p   = −dt·v·sin(p)·cos(ψ)
     ∂f₀/∂ψ   = −dt·v·cos(p)·sin(ψ)

   f₁ = y + dt·v·( cos(r)·sin(ψ) + sin(r)·sin(p)·cos(ψ) )
     ∂f₁/∂r   =  dt·v·(−sin(r)·sin(ψ) + cos(r)·sin(p)·cos(ψ))
     ∂f₁/∂p   =  dt·v·  sin(r)·cos(p)·cos(ψ)
     ∂f₁/∂ψ   =  dt·v·( cos(r)·cos(ψ) − sin(r)·sin(p)·sin(ψ))

   f₂ = z + dt·v·( sin(r)·sin(ψ) − cos(r)·sin(p)·cos(ψ) )
     ∂f₂/∂r   =  dt·v·( cos(r)·sin(ψ) + sin(r)·sin(p)·cos(ψ))
     ∂f₂/∂p   = −dt·v·  cos(r)·cos(p)·cos(ψ)
     ∂f₂/∂ψ   =  dt·v·( sin(r)·cos(ψ) + cos(r)·sin(p)·sin(ψ))

   f₃ = r,  f₄ = p,  f₅ = ψ + dt·ω   →  diagonal 1, no cross terms.
*/
void sys_evaluate_A( double A[6][6], const State& state, double v, double w, double dt ){

  for( int r=0; r<6; r++ )
    for( int c=0; c<6; c++ )
      A[r][c] = 0.0;

  // Identity diagonal
  for( int i=0; i<6; i++ ) A[i][i] = 1.0;

  const double r_   = state.x[State::ROT_R];
  const double p_   = state.x[State::ROT_P];
  const double psi_ = state.x[State::ROT_Y];

  const double cr = std::cos(r_),   sr = std::sin(r_);
  const double cp = std::cos(p_),   sp = std::sin(p_);
  const double cy = std::cos(psi_), sy = std::sin(psi_);

  const double dtv = dt * v;

  // Row 0  (POS_X)
  A[State::POS_X][State::ROT_P] = -dtv * sp * cy;
  A[State::POS_X][State::ROT_Y] = -dtv * cp * sy;

  // Row 1  (POS_Y)
  A[State::POS_Y][State::ROT_R] =  dtv * (-sr*sy + cr*sp*cy);
  A[State::POS_Y][State::ROT_P] =  dtv * ( sr*cp*cy);
  A[State::POS_Y][State::ROT_Y] =  dtv * ( cr*cy - sr*sp*sy);

  // Row 2  (POS_Z)
  A[State::POS_Z][State::ROT_R] =  dtv * ( cr*sy + sr*sp*cy);
  A[State::POS_Z][State::ROT_P] = -dtv * ( cr*cp*cy);
  A[State::POS_Z][State::ROT_Y] =  dtv * ( sr*cy + cr*sp*sy);

  // Rows 3,4,5  (ROT_R, ROT_P, ROT_Y) – already set to identity above, no cross terms
}

/**
   GPS observation function  h_gps(s) = [latitude, longitude, altitude].

   Flat-earth linear model (ENU frame: X=East, Y=North, Z=Up):
     latitude  = lat₀ + y / M_PER_DEG_LAT
     longitude = lon₀ + x / M_PER_DEG_LON
     altitude  = alt₀ + z
*/
sensor_msgs::msg::NavSatFix meas_evaluate_gps( const State& state ){

  sensor_msgs::msg::NavSatFix nsf;

  const double x = state.x[State::POS_X];   // East  (m)
  const double y = state.x[State::POS_Y];   // North (m)
  const double z = state.x[State::POS_Z];   // Up    (m)

  nsf.latitude  = GPS_LAT0 + y / M_PER_DEG_LAT;
  nsf.longitude = GPS_LON0 + x / M_PER_DEG_LON;
  nsf.altitude  = GPS_ALT0 + z;

  return nsf;
}

/**
   IMU observation function  h_imu(s) = [roll, pitch, yaw].

   The EKF state already stores RPY, so this is an identity-like readout.
   (The ekf_node converts the IMU quaternion to RPY using the same
    Rx·Ry·Rz convention as utilities.cpp before comparing to this prediction.)
*/
sensor_msgs::msg::RPY meas_evaluate_imu( const State& state ){
  sensor_msgs::msg::RPY rpy;

  rpy.roll  = state.x[State::ROT_R];
  rpy.pitch = state.x[State::ROT_P];
  rpy.yaw   = state.x[State::ROT_Y];

  return rpy;
}

/**
   3×3 GPS Jacobian  H_gps = ∂h_gps / ∂[x, y, z].

   From the flat-earth model:
     ∂lat/∂x = 0,                 ∂lat/∂y = 1/M_PER_DEG_LAT, ∂lat/∂z = 0
     ∂lon/∂x = 1/M_PER_DEG_LON,  ∂lon/∂y = 0,                ∂lon/∂z = 0
     ∂alt/∂x = 0,                 ∂alt/∂y = 0,                ∂alt/∂z = 1

   Assembled into the full 6×6 H by measurement_pdf.cpp at rows 1-3, cols 1-3.
*/
void meas_evaluate_Hgps( double Hgps[3][3], const State& state ){

  for( int r=0; r<3; r++ )
    for( int c=0; c<3; c++ )
      Hgps[r][c] = 0.0;

  Hgps[0][1] = 1.0 / M_PER_DEG_LAT;   // ∂lat / ∂y
  Hgps[1][0] = 1.0 / M_PER_DEG_LON;   // ∂lon / ∂x
  Hgps[2][2] = 1.0;                   // ∂alt / ∂z
}

/**
   3×3 IMU Jacobian  H_imu = ∂h_imu / ∂[r, p, ψ]  =  I₃.

   The IMU observation is a direct readout of state RPY, so the Jacobian
   is the 3×3 identity matrix.
   Assembled into the full 6×6 H by measurement_pdf.cpp at rows 4-6, cols 4-6.
*/
void meas_evaluate_Himu( double Himu[3][3], const State& state ){

  for( int r=0; r<3; r++ )
    for( int c=0; c<3; c++ )
      Himu[r][c] = 0.0;

  Himu[0][0] = 1.0;
  Himu[1][1] = 1.0;
  Himu[2][2] = 1.0;
}
