// Copyright (C) 2008 Wim Meeussen <meeussen at willowgarage com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "system_pdf.hpp"
#include "ekf_models.hpp"
#include <bfl/wrappers/rng/rng.h> // Wrapper around several rng libraries

namespace BFL
{

  using namespace MatrixWrapper;
  
  SystemPDF::SystemPDF( const Gaussian& additiveNoise )
    : AnalyticConditionalGaussianAdditiveNoise( additiveNoise, 2 ){}
  
  SystemPDF::~SystemPDF(){}


  // This method gets called from AnalyticSystemModelGaussianUncertainty::PredictionGet
  // Argument 0 is set to x (posterior expected value)
  // Argument 1 is set to u
  ColumnVector SystemPDF::ExpectedValueGet() const{
    
    ColumnVector state = ConditionalArgumentGet(0);
    ColumnVector vel = ConditionalArgumentGet(1);

    // copy to the state (1 indexed to 0 indexed)
    State state_in;
    for( size_t i=1; i<=6; i++ )
      { state_in.x[i-1] = state(i); }

    // Call the state prediction
    State state_out = sys_evaluate_f( state_in, vel(1), vel(2), vel(3) );

    // copy back to the state
    for( size_t i=1; i<=6; i++ )
      { state(i) = state_out.x[i-1]; }

    return state;

  }


  // This method gets called from AnalyticSystemModelGaussianUncertainty::df_dxGet
  // Argument 0 is set to x (posterior expected value)
  // Argument 1 is set to u
  Matrix SystemPDF::dfGet(unsigned int i) const{

    Matrix df( 6, 6 );
    // initialize df matrix to identity
    for( int r=1; r<=6; r++){
      for( int c=1; c<=6; c++){
        if( r == c ) { df(r,c) = 1; }
        else         { df(r,c) = 0; }
      }
    }

    if( i==0 ){

      ColumnVector state = ConditionalArgumentGet(0);
      ColumnVector vel = ConditionalArgumentGet(1);

      // copy to the state
      State s;
      for( size_t i=1; i<=6; i++ )
	{ s.x[i-1] = state(i); }

      double G[6][6];
      sys_evaluate_A( G, s, vel(1), vel(2), vel(3) );

      // initialize df matrix
      for( int r=1; r<=6; r++){
	for( int c=1; c<=6; c++){
	  df(r,c) = G[r-1][c-1];
	}
      }
    }

    return df;

  }

  MatrixWrapper::SymmetricMatrix SystemPDF::CovarianceGet() const{

    ColumnVector state = ConditionalArgumentGet(0);
    ColumnVector vel = ConditionalArgumentGet(1);
    
    // copy to the state
    State s;
    for( size_t i=1; i<=6; i++ )
      { s.x[i-1] = state(i); }

    double Q[6][6];
    sys_evaluate_WMWt( Q, s, vel(1), vel(2), vel(3) );
    
    SymmetricMatrix sysQ( 6 );
    for( int r=1; r<=6; r++){
      for( int c=1; c<=6; c++){
	sysQ(r,c) = Q[r-1][c-1];
      }
    }

    return sysQ;
  }
  
}//namespace BFL

