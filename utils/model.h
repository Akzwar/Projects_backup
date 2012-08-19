#include "quat.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "randn.h"
#pragma once
template<class DD> class Model 
{
 protected:
	Vect<DD> IV;
    DD mu;
 public:
	Model(){}
    Model(DD p, DD e, DD i, DD Omega, DD omega, DD Theta)
    {
        mu = 3.986004418e5 * 3600 * 3600;
        IV = Vect<DD>( 6 );
        DD R = p / ( 1 + e * cos( Theta ));
        DD u = Theta + omega;

        IV[V_X] = R * ( cos( u ) * cos( Omega ) - sin( u ) * sin( Omega ) * cos( i ) );
        IV[V_Y] = R * ( cos( u ) * sin( Omega ) - sin( u ) * cos( Omega ) * cos( i ) );
        IV[V_Z] = R * sin( u ) * sin( i );

        DD Vr = sqrt( mu / p ) * e * sin( Theta ) / R;
        DD Vn = sqrt( mu / p ) * ( 1 + e * cos( Theta) );

        IV[V_dX] = IV[V_X] * Vr + Vn * ( -sin( u ) * cos( Omega ) 
                                        -cos( u ) * sin( Omega ) * cos( i ) );
        IV[V_dY] = IV[V_Y] * Vr + Vn * ( -sin( u ) * sin( Omega ) 
                                        +cos( u ) * cos( Omega ) * cos( i ) );
        IV[V_dZ] = IV[V_Z] * Vr + Vn *cos( u ) * sin( i );
        this->IV.print();
    } 
	virtual const Vect<DD> getRight( const Vect<DD>& Vt, DD t ){}
	Vect<DD> InitVect(){ return IV; }
};

template<class DD> class KeplerModel : public Model<DD>
{
 public:
    KeplerModel() : Model<DD>(){}
	KeplerModel( double R ) : Model<DD>()
	{  
		this->mu = 2.9591220829566038481357324012661e-4;
		this->IV = Vect<DD>( 6 );
		this->IV[ V_X ] = 0.11601490913916648627 * R;
		this->IV[ V_Y ] = -0.92660555364038517604 * R;
		this->IV[ V_Z ] = -0.40180627760698804496 * R;
		this->IV[ V_dX ] = 0.01681162005220228976 * R;
		this->IV[ V_dY ] = 0.00174313168798203152 * R;
		this->IV[ V_dZ ] = 0.00075597376713614610 * R; 
	}	
    KeplerModel(DD p, DD e, DD i, DD Omega, DD omega, DD Theta) : Model<DD>( p,e,i,Omega,omega,Theta ){}

	virtual const Vect<DD> getRight( const Vect<DD>& Vt, DD t )
	{
		
		Vect<DD> V(Vt);
		Vect<DD> retVect( V.size() );
		DD R = sqrt( V[V_X] * V[V_X] + V[V_Y] * V[V_Y] + V[V_Z] * V[V_Z] );
		retVect[V_X] = V[V_dX];
		retVect[V_Y] = V[V_dY];
		retVect[V_Z] = V[V_dZ];
		retVect[V_dX] = -this->mu * V[V_X]/( R * R * R );
		retVect[V_dY] = -this->mu * V[V_Y]/( R * R * R );
		retVect[V_dZ] = -this->mu * V[V_Z]/( R * R * R );
		return retVect;
	}
};

template<class DD>
class DotMassesModel : public Model<DD>
{
 private:
    struct MassDot
    {
	    DD mass,X,Y,Z;
    };
    static const int MDCount = 7;
	MassDot MD[MDCount];
	DD Emass;
	DD aem;
 public:
	DotMassesModel() : Model<DD>()
	{	
		Emass = 5.9742*10e23;
		MD[0].mass =  10652.73; MD[0].X = -4200.917; MD[0].Y = 3067.194;  MD[0].Z = -217.754;
		MD[1].mass =  -4160.24; MD[1].X = 856.705;   MD[1].Y = 5487.255;  MD[1].Z = -599.402;
		MD[2].mass =  41357.43; MD[2].X = 2843.858;  MD[2].Y = -1911.723; MD[2].Z = 3151.231;
		MD[3].mass = -48929.27; MD[3].X = 1469.622;  MD[3].Y = -3260.508; MD[3].Z = 2157.722;
		MD[4].mass =   3856.77; MD[4].X = 2825.441;  MD[4].Y = 2630.083;  MD[4].Z = -4148.659;
		MD[5].mass = -50642.30; MD[5].X = -1259.093; MD[5].Y = 271.749;   MD[5].Z = -3997.510;
		MD[6].mass =   6602.59; MD[6].X = -5413.541; MD[6].Y = 531.582;	  MD[6].Z = -1948.403;
		MD[7].mass = -19965.52; MD[7].X = 184.686;   MD[7].Y = 4435.039;  MD[7].Z = 3698.865;
        for( int i = 0; i < MDCount; i++)
            MD[i].mass *= 10e10;
	}
    DotMassesModel(DD p, DD e, DD i, DD Omega, DD omega, DD Theta) : Model<DD>( p,e,i,Omega,omega,Theta ) 
	{
		Emass = 5.9742*10e24;
		/*MD[0].mass =  10652.73; MD[0].X = -4200.917; MD[0].Y = 3067.194;  MD[0].Z = -217.754;
		MD[1].mass =  -4160.24; MD[1].X = 856.705;   MD[1].Y = 5487.255;  MD[1].Z = -599.402;
		MD[2].mass =  41357.43; MD[2].X = 2843.858;  MD[2].Y = -1911.723; MD[2].Z = 3151.231;
		MD[3].mass = -48929.27; MD[3].X = 1469.622;  MD[3].Y = -3260.508; MD[3].Z = 2157.722;
		MD[4].mass =   3856.77; MD[4].X = 2825.441;  MD[4].Y = 2630.083;  MD[4].Z = -4148.659;
		MD[5].mass = -50642.30; MD[5].X = -1259.093; MD[5].Y = 271.749;   MD[5].Z = -3997.510;
		MD[6].mass =   6602.59; MD[6].X = -5413.541; MD[6].Y = 531.582;	  MD[6].Z = -1948.403;
		MD[7].mass = -19965.52; MD[7].X = 184.686;   MD[7].Y = 4435.039;  MD[7].Z = 3698.865;
		MD[8].mass = 32416.19;  MD[8].X = 1785.481;  MD[8].Y = -3390.893; MD[8].Z = -2863.121;
        */
        MD[0].mass = 7.53770461; MD[0].X = 0; MD[0].Y = 0; MD[0].Z = 0;
        MD[1].mass = -1.62110461; MD[1].X = 0; MD[1].Y = 0; MD[1].Z = 300;
        MD[2].mass = -1.62110461; MD[2].X = 0; MD[2].Y = 0; MD[2].Z = -300;
        MD[3].mass = 5.88921919; MD[3].X = 0; MD[3].Y = 0; MD[3].Z = 250;
        MD[4].mass = 5.88921919; MD[4].X = 0; MD[4].Y = 0; MD[4].Z = -250;
        MD[5].mass = -7.53696689; MD[5].X = 0; MD[5].Y = 0; MD[5].Z = 180;
        MD[6].mass = -7.53696680; MD[6].X = 0; MD[6].Y = 0; MD[6].Z = -180;
        //for( int i = 0; i < MDCount; i++)
          //  MD[i].mass *= 10e10;
	}

	virtual const Vect<DD> getRight( const Vect<DD>& Vt, DD t )
	{
		Vect<DD> V( Vt );
		Vect<DD> retVect( V.size() );
		Vect<DD> summ( 3 );
		DD dX, dY, dZ, R, eps, X, Y, Z;
		for( int i = 0; i < MDCount; i++ )
		{
			dX = MD[i].X - V[V_X];
			dY = MD[i].Y - V[V_Y];
			dZ = MD[i].Z - V[V_Z];
			R =  sqrt( dX * dX + dY * dY + dZ * dZ ) ;
			eps = MD[i].mass; // (double)Emass;
			dX = eps * dX / (R*R*R);
			dY = eps * dY / (R*R*R);
			dZ = eps * dZ / (R*R*R);
			summ += Vect<DD>( dX, dY, dZ ) ; 
		}
		summ = summ * this->mu;
		retVect[V_X] = V[V_dX];
		retVect[V_Y] = V[V_dY];
		retVect[V_Z] = V[V_dZ];
		retVect[V_dX] = summ[V_X];
		retVect[V_dY] = summ[V_Y];
		retVect[V_dZ] = summ[V_Z];
		return retVect;	
	}
};

	
double LegandrSin( int n, int m, double Phi )
{
	double sm;
	if( (m - 1) == 0 )
		sm = 0.5;
	else 
		sm = 1;
	if( n < m )
		return 0;
	if( n == m && m == 0 )
		return 1;
	if( n == m && m!=0 )
		return LegandrSin( n-1, m-1, Phi ) * cos(Phi) * sqrt( (2*n+1)/(double)(2*n)*1/sm );
	return LegandrSin( n-1, m, Phi ) * sin(Phi) * sqrt( (4*n*n - 1)/(double)(n*n-m*m) ) - LegandrSin( n-2, m, Phi ) * sqrt((( (n-1)*(n-1) )-m*m)*(2*n+1)/(double)((n*n-m*m)*(2*n-3)));
}

double dLegandrSin( int n, int m, double Phi )
{
	double sm;
	if( m == 0 )
		sm = 0.5;
	else 
		sm = 1;
	return -( m * tan(Phi) * LegandrSin( n, m, Phi ) - sqrt( sm*(n-m)*(n+m+1) ) * LegandrSin( n, m+1, Phi ) );
}

template<typename DD>
class NormalSphereFuncModel : public Model<DD>
{
 private:
	static const double ae = 6378.136;
	static const long double C20 = -484165e-9;
	static const long double C40 = 790.3e-9;	
 public:
	NormalSphereFuncModel() : Model<DD>()
	{
	}
    NormalSphereFuncModel(DD p, DD e, DD i, DD Omega, DD omega, DD Theta) : Model<DD>( p,e,i,Omega,omega,Theta )
    {
    }

	virtual const Vect<DD> getRight( const Vect<DD>& Vt, DD t )
	{
		Vect<DD> V( Vt );
		Vect<DD> retVect( V.size() );
		Vect<DD> dV0( V.size() );
		DD Ro, Phi, Lambda, dPhi, dRo, dLambda;
		DD r0 = sqrt( V[V_X] * V[V_X] + V[V_Y] * V[V_Y] );
		Ro = V.Length();
		Phi = atan2( V[V_Z], r0 );
		Lambda = atan2( V[V_Y], V[V_X] );
        DD er = ae / Ro;
        DD k = this->mu / Ro / ae;
        dRo = -k*( 3*pow(er,3)*C20*LegandrSin(2,0,Phi)
                                + 5*pow(er,5)*C40*LegandrSin(4,0,Phi));
        dPhi = k*( pow(er,3)*C20*dLegandrSin(2,0,Phi) * cos(Phi)
                                + pow(er,5)*C40*dLegandrSin(4,0,Phi) * cos(Phi));
		dLambda = 0;
        dRo += -this->mu / (Ro * Ro);
		dV0 = Vect<DD>(dRo, dPhi, dLambda);
		Matrix<DD> MdV0;
		MdV0.addRow(dV0);
		MdV0.Transpose();
		Matrix<DD> MP(3,3);
		MP[0][0] = V[V_X] / Ro; MP[0][1] = -V[V_X] * V[V_Z] / ( Ro * r0 ); MP[0][2] = -V[V_Y] / r0;
		MP[1][0] = V[V_Y] / Ro; MP[1][1] = -V[V_Y] * V[V_Z] / ( Ro * r0 ); MP[1][2] = V[V_X] / r0;
		MP[2][0] = V[V_Z] / Ro; MP[2][1] = -r0 / Ro;                       MP[2][2] = 0;
		Matrix<DD> res = MP * MdV0;
		retVect[V_X] = V[ V_dX ];
		retVect[V_Y] = V[ V_dY ];
		retVect[V_Z] = V[ V_dZ ];
		retVect[V_dX] = res[0][0];
		retVect[V_dY] = res[1][0];
		retVect[V_dZ] = res[2][0];
		return retVect;
	}
};

template<class DD>
class AnomalSphereFuncModel : public Model<DD>
{
 private:
	double C[7][7],S[7][7];
	static const double ae = 6378.136;
 public:
	AnomalSphereFuncModel() : Model<DD>()
	{
        for(int i = 0; i < 7; i++)
		    for( int j =0; j<7;j++ )
			{
				C[i][j] = 0;
				S[i][j] = 0;
			}
		C[1][0] = 0; C[1][1] = 0; 
		C[2][0] = -484164.95;	C[2][1] = 0.05; C[2][2] = 2438.76;
		C[3][0] = 957.16; C[3][1] = 2032.83; C[3][2] = 906.86; C[3][3] = 715.99;
		C[4][0] = 543.52; C[4][1] = -531.34; C[4][2] = 350.04; C[4][3] = 989.28; C[4][4] = -193.25;
		C[5][0] = 68.72;  C[5][1] = -67.36;  C[5][2] = 651.94; C[5][3] = -451.45; C[5][4] = -293.19;
		C[5][5] = 191.57; 
		C[6][0] = -143.45; C[6][1] = -77.66; C[6][2] = 38.02; C[6][3] = 56.65; C[6][4] = -90.89;
		C[6][5] = -268.71; C[6][6] = 14.97;
		S[1][1] = 0.0; S[2][1] = 0.01; S[2][2] = -1399.68; S[3][1] = 249.03; S[3][2] = -619.29;
		S[3][3] = 1400.74; S[4][1] = -471.95; S[4][2] = 651.66; S[4][3] = -197.67; S[4][4] = 298.76;
		S[5][1] = -84.56; S[5][2] = -326.28; S[5][3] = -201.09; S[5][4] = 54.38; S[5][5] = -669.62;
		S[6][1] = 32.21; S[6][2] = -364.79; S[6][3] = 2.64; S[6][4] = -465.16; S[6][5] = -535.01;
		S[6][6] = -243.90;
	}
    AnomalSphereFuncModel(DD p, DD e, DD i, DD Omega, DD omega, DD Theta) : Model<DD>( p,e,i,Omega,omega,Theta )
    {
        for(int i = 0; i < 7; i++)
		    for( int j =0; j<7;j++ )
			{
				C[i][j] = 0;
				S[i][j] = 0;
			}
		C[1][0] = 0; C[1][1] = 0; 
		C[2][0] = -484164.95;	C[2][1] = 0.05; C[2][2] = 2438.76;
		C[3][0] = 957.16; C[3][1] = 2032.83; C[3][2] = 906.86; C[3][3] = 715.99;
		C[4][0] = 543.52; C[4][1] = -531.34; C[4][2] = 350.04; C[4][3] = 989.28; C[4][4] = -193.25;
		C[5][0] = 68.72;  C[5][1] = -67.36;  C[5][2] = 651.94; C[5][3] = -451.45; C[5][4] = -293.19;
		C[5][5] = 191.57; 
		C[6][0] = -143.45; C[6][1] = -77.66; C[6][2] = 38.02; C[6][3] = 56.65; C[6][4] = -90.89;
		C[6][5] = -268.71; C[6][6] = 14.97;
		S[1][1] = 0.0; S[2][1] = 0.01; S[2][2] = -1399.68; S[3][1] = 249.03; S[3][2] = -619.29;
		S[3][3] = 1400.74; S[4][1] = -471.95; S[4][2] = 651.66; S[4][3] = -197.67; S[4][4] = 298.76;
		S[5][1] = -84.56; S[5][2] = -326.28; S[5][3] = -201.09; S[5][4] = 54.38; S[5][5] = -669.62;
		S[6][1] = 32.21; S[6][2] = -364.79; S[6][3] = 2.64; S[6][4] = -465.16; S[6][5] = -535.01;
		S[6][6] = -243.90;
        for(int i = 0; i < 7; i++)
		    for( int j =0; j<7;j++ )
            {
                C[i][j] *= 1e-9;
                S[i][j] *= 1e-9;
            }
    }
    
	virtual const Vect<DD> getRight( const Vect<DD>& Vt, DD t )
	{
		int N = 7;
		DD summ1 = 0;
		DD summ2 = 0;
		Vect<DD> V(Vt);
		Vect<DD> retVect( V.size() );
		DD Ro, Phi, Lambda, dPhi, dRo, dLambda;
		DD r0 = sqrt( V[V_X] * V[V_X] + V[V_Y] * V[V_Y] );
		Ro = V.Length();
		Phi = atan2( V[V_Z], r0 );
		Lambda = atan2( V[V_Y], V[V_X] );
		for( int n = 2; n < N; n++ )
		{
			summ2 = 0;
			for( int m = 0; m <= n; m++ )
				summ2 += (C[n][m]*cos(m*Lambda) + S[n][m]*sin(m*Lambda))*LegandrSin(n,m,Phi);
			summ1 += (n+1)*pow(ae/Ro,n+1)*summ2;
		}
		dRo = -this->mu * summ1 / (ae*Ro);
		summ1 = 0;
		for( int n = 2; n < N; n++ )
		{
			summ2 = 0;
			for( int m = 0; m <= n; m++ )
				summ2 += (C[n][m]*cos(m*Lambda) + S[n][m]*sin(m*Lambda))*dLegandrSin(n,m,Phi);
			summ1 += pow(ae/Ro,n+1)*summ2;
		}
		dPhi = -this->mu * summ1 / (ae*Ro);
		summ1 = 0;
		for( int n = 2; n < N; n++ )
		{
			summ2 = 0;

			for( int m = 0; m <= n; m++ )
				summ2 += (-C[n][m]*sin(m*Lambda) + S[n][m]*cos(m*Lambda))*m*LegandrSin(n,m,Phi);
			summ1 += pow(ae/Ro,n+1)*summ2;
		}
		dLambda = -this->mu * summ1 / (ae*Ro*cos(Phi));			
		Matrix<DD> MP(3,3);
                MP[0][0] = V[V_X] / Ro; MP[0][1] = -V[V_X] * V[V_Z] / ( Ro * r0 ); MP[0][2] = -V[V_Y] / r0;
                MP[1][0] = V[V_Y] / Ro; MP[1][1] = -V[V_Y] * V[V_Z] / ( Ro * r0 ); MP[1][2] = V[V_X] / r0;
                MP[2][0] = V[V_Z] / Ro; MP[2][1] = -r0 / Ro;                       MP[2][2] = 0;
		Matrix<DD> gS;
        Vect<double> dG0(dRo, dPhi, dLambda);
        //dG0.print();
		gS.addRow(dG0);
		gS.Transpose();
		Matrix<DD> res = MP * gS;
		retVect[V_X] = V[V_dX];
		retVect[V_Y] = V[V_dY];
		retVect[V_Z] = V[V_dZ];
		retVect[V_dX] = V[V_X]*(-this->mu/pow(V.Length(),3))+res[0][0];
		retVect[V_dY] = V[V_Y]*(-this->mu/pow(V.Length(),3))+res[1][0]; 
		retVect[V_dZ] = V[V_Z]*(-this->mu/pow(V.Length(),3))+res[2][0];
		return retVect;
	}	
};


long double WNoise( double dt )
{
    return randn_trig(0,sqrt(1.0/dt));
    //return 1/sqrt(dt) * ( rand() / (double)RAND_MAX - 0.5 );
}

template<typename DD>
class ShapingFilter : public Model<DD>
{
 private:
    struct { DD K, T; } sw1;
    struct { DD K, T1, T2, Nu, dtNu, TT2; } sw2;
    struct { DD K, T1, T2, TT2; } sw3;
	DD D, lambda, beta;
    void SetVars()
    {
        DD sigma = sqrt( D ); 
        sw1.K = sigma * sqrt( 2.0 / lambda );
        sw1.T = 1.0 / lambda;
        DD l2b2 = lambda * lambda + beta * beta;
        //sw2.K = sigma * sqrt( 2.0 * lambda / l2b2 );
        sw2.K = sigma * sqrt( lambda / sqrt( l2b2 ) );
        sw2.T1 = sqrt( 1.0 / l2b2 );
        sw2.T2 = sw2.T1;
        sw2.TT2 = sw2.T2 * sw2.T2;
        sw2.Nu = sqrt( ( lambda * lambda - beta * beta ) / ( 2.0 * l2b2 * l2b2 ) + 0.5 );
        sw2.dtNu = 2.0 * sw2.T2 * sw2.Nu;
        sw3.K = sigma * sqrt( 2.0 / lambda );
        sw3.T1 = sqrt( fabs( 1.0 - lambda ) / pow( lambda, 3 ) );
        sw3.T2 = 1.0 / lambda;
        sw3.TT2 = sw3.T2 * sw3.T2;
    }
    static const int w1 = 0;
    static const int w2 = 1;
    static const int w3 = 4;

    static const int w2q = 2;
    static const int w2r= 3;

    static const int w3q = 5;
    static const int w3r = 6;
 public:
	ShapingFilter(DD _D, DD _lambda, DD _beta) : Model<DD>()
	{
		this->IV = Vect<DD>(7);
        this->IV[0] = 0;
		D = _D;
		lambda = _lambda;
        beta = _beta;
        SetVars();
        srand(time(NULL));
	}
	virtual const Vect<DD> getRight( const Vect<DD>& Vt, DD t )
	{
        DD wn[3];
		Vect<DD> V(Vt);
        Vect<double> tmpVect(5);
		Vect<DD> retVect(V.size());
        
        for( int i = 0; i < 3; i++ )
            wn[i] = WNoise(t);

        tmpVect[0] = (  sw1.K * wn[0] - V[w1] ) / sw1.T;

        tmpVect[1] = sw2.K * ( sw2.T1 * V[w2r] + V[w2q] );
        tmpVect[2] = ( wn[1] - V[w2r] * sw2.dtNu - V[w2q] ) / sw2.TT2;;

        tmpVect[3] = sw3.K * ( sw3.T1 * V[w3r] + V[w3q] );
        tmpVect[4] = ( wn[2] - V[w3r] * sw3.T2 * 2 - V[w3q] ) / sw3.TT2;

        retVect[w1] = tmpVect[0];

        retVect[w2] = tmpVect[1]/t;
        retVect[w2q] = V[w2r];
        retVect[w2r] = tmpVect[2];

        retVect[w3] = tmpVect[3]/t;
        retVect[w3q] = V[w3r];
        retVect[w3r] = tmpVect[4];
        
		return retVect;
	}
};
