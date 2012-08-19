#pragma once

#include "ofMain.h"
#include <vector>
#include <iostream>
using namespace std;

long double maxld(long double a, long double b);

long double minld(long double a, long double b);


class Vect
{
    public:

        Vect()
        {
		Up = 0;
		dUp = 0;
		U = 0;
		dU = 0;
        }

        Vect(const Vect &V)
        {
		Up = V.Up;
		dUp = V.dUp;
		U = V.U;
		dU = V.dU;
        }

	Vect(long double _Up, long double _dUp, long double _U, long double _dU)
        {
		Up = _Up;
		dUp = _dUp;
		U = _U;
		dU = _dU;
        }

	long double Up, dUp, U, dU;

        Vect operator + (const Vect& V) const
        {
		Vect retVect;
		retVect.Up  = V.Up  + this->Up;
		retVect.dUp = V.dUp + this->dUp;
		retVect.U = V.U + this->U;
		retVect.dU = V.dU + this->dU;
        	return retVect;
        }

        void operator += (const Vect& V)
        {	    
        	this->Up += V.Up;
		this->dUp += V.dUp;
		this->U += V.U;
		this->dU += V.dU;
        }

        Vect operator * (const long double Cross) const
        {
        	Vect retVect;
	    
        	retVect.Up = this->Up * Cross;
		retVect.dUp = this->dUp * Cross;
		retVect.U = this->U * Cross;
		retVect.dU = this->dU * Cross;
	    
        	return retVect;
        }

        bool operator == (const Vect& V) const
        {
            if(( this->Up  != V.Up )||
		( this->dUp != V.dUp  )||
		( this->U != V.U )||
		( this->dU != V.dU ))
                    return false;
            return true;
        }

        bool operator != (const Vect& V) const
        {
            return !((*this)==V);
        }

        inline void operator = (const Vect& V)
        {
   		this->Up  = V.Up;
		this->dUp = V.dUp;
		this->U   = V.U;
		this->dU  = V.dU;
        }

        //double Angle()const {return atan2(Y2, long double Y1);}
};


class Model
{
    private:
	long double Kekv, K, _Up, _dUp, _U, _dU, ddU , Eps, Uy, fEps, Kk, Tk, Iya, Kp, Td, Kd, dEps, pEps, s, z, h;
    public:
        Model()
        {
        	InitVect = Vect(0,0,0.0,0);
		s = 15.0;
		z = 1.0;
		h = 1.0;
		Kk = 1.0;
		Tk = 0.00021;
		Kp = 0.02;
		Kd = 100.0;
		Td = 0.02;
		Uy = 10.0;
		pEps = 0.0;
		Kekv = 1 + Kk*Kd*Kp;
		K = Kk*Kd*Kp/Kekv;
        }
        Vect InitVect;
        Vect getRight(const Vect& V, long double t )
        {
        	Vect retVect;
		_U = V.U;
		_dU = V.dU;
	 	
		Eps = Uy - _U;
	    	Iya = _Up * Kp;
		dEps = ( Eps - pEps)/t;

		//printf("%f,%f,%f\n",(double)Eps,(double)pEps,(double)dEps);
		if( dEps > 0 )
			{
				if( Eps < -s - z/2.0 )
					fEps = -s;
				if( Eps >= -s - z/2.0 && Eps < -z/2.0 )
					fEps = - ( Eps - z/2.0 );
				if( Eps >= -z/2.0 && Eps <= z/2.0 + h )
					fEps = 0;
				if( Eps >= z/2.0 + h && Eps <= z/2.0 + h + s )
					fEps = Eps - z/2.0 - h;
				if( Eps > z/2.0 + h + s )
					fEps = s;
			}
		if( dEps <= 0 )
			{	
				if( Eps < -z/2.0 - h - s )
					fEps = -s;
				if( Eps >= -z/2.0 -h -s && Eps < -z/2.0 - h )
					fEps = -( Eps - z/2.0 - h );
				if( Eps >= -h - z/2.0 && Eps < z/2.0 )
					fEps = 0;
				if( Eps >= z/2.0 && Eps < z/2.0 + s )
					{
						fEps = Eps - z/2.0;
					}
				if( Eps >= z/2.0 + s )
					fEps = s;
			}
		ddU = (K*Uy -2*(Tk*Td/Kekv)*_dU-_U) / (Tk*Td) * Kekv * 2 ;	 

           	retVect.U = _dU;
	    	retVect.dU = ddU ;
		pEps = Eps;
            	return retVect;
        }
};
class Integrator
{
    private:
        long double t;
        Vect k1,k2,k3,k4,k5,k6,k7;


        long double a[8][7];
        long double b[7];
        long double b1[7];
        long double c[7];

        long double eps, eps_max, u, eps1, eps2;

        Vect PrevVect;
        Vect tmpVect;
        Vect epsVect;
    public:
        Integrator(){}

        Integrator(Model& _M, long double _t0, long double _tk, long double _step, long double _h)
        {
		Tout = 0;
            eps=0;
            eps_max = 10e-20;
            long double v=1;
            while ( ( 1 + v ) > 1 )
            {
                u=v;
                v=v/2;
            }
            t  = _t0 ;
            M  = _M ;
            t0 = _t0;
            tk = _tk;
            h = _h;
	    step = _step;
            CurrVect = M.InitVect ;
            a[2][1]=1.0/5.0;
            a[3][1]=3.0/40.0;
            a[3][2]=9.0/40.0;
            a[4][1]=44.0/45.0;
            a[4][2]=-56.0/15.0;
            a[4][3]=32.0/9.0;
            a[5][1]=19372.0/6561.0;
            a[5][2]=-25360.0/2187.0;
            a[5][3]=64448.0/6561.0;
            a[5][4]=-212.0/729.0;
            a[6][1]=9017.0/3168.0;
            a[6][2]=-355.0/33.0;
            a[6][3]=46732.0/5247.0;
            a[6][4]=49.0/176.0;
            a[6][5]=-5103.0/18656.0;
            a[7][1]=35.0/384.0;
            a[7][2]=0;
            a[7][3]=500.0/1113.0;
            a[7][4]=125.0/192.0;
            a[7][5]=-2187.0/6784.0;
            a[7][6]=11.0/84.0;
            b[1]=35.0/384.0;
            b[2]=0;
            b[3]=500.0/1113.0;
            b[4]=125.0/192.0;
            b[5]=-2187.0/6784.0;
            b[6]=11.0/84.0;
            b[7]=0;
            b1[1]=5179.0/57600.0;
            b1[2]=0;
            b1[3]=7571.0/16695.0;
            b1[4]=393.0/640.0;
            b1[5]=-92097.0/339200.0;
            b1[6]=187.0/2100.0;
            b1[7]=1.0/40.0;
            c[1]=0;
            c[2]=1.0/5.0;
            c[3]=3.0/10.0;
            c[4]=4.0/5.0;
            c[5]=8.0/9.0;
            c[6]=1.0;
            c[7]=1.0;
		NextStep();
        }
        Model M;
        Vect CurrVect;
	Vect OutVect;
        long double getT(){return t;}
        long double getT0(){return t0;}
        long double getTk(){return tk;}
        long double getStep(){return step;}
	long double getSamplingIncrement(){return h;}
        long double t0;
        long double tk;
        long double step;
	long double h;
	long double Tout;
	void SmallStep()
	{
		if(Tout > step )
		{
			NextStep();
			Tout=0;
		}
		double teta = h/step;
		b[1]=teta*(1+teta*(-1337.0/480.0 + teta*(1039.0/360.0 + teta*(-1163.0/1152.0))));
		b[2]=0.0;
		b[3] = 100.0 * teta * teta * (1054.0/9275.0 + teta* ( -4682.0/27825.0 + teta*( 379.0/5565.0 ) ) )/3.0; 
		b[4] = -5.0*teta*teta*( 27.0/40.0 + teta * (-9.0/5.0 + teta *(83.0/96.0)) )/2.0;
		b[5] = 18225.0*teta*teta*(-3.0/250.0+teta*(22.0/375.0+teta*(-37.0/600.0)))/848.0;
		b[6] = -22.0*teta*teta*(-3.0/10.0+teta*(29.0/30.0+teta*(-17.0/24.0)))/7.0;
		OutVect = (( k1 * b[1] ) +
                            ( k2 * b[2] ) +
                            ( k3 * b[3] ) +
                            ( k4 * b[4] ) +
                            ( k5 * b[5] ) +
                            ( k6 * b[6] ) +
                            ( k7 * b[7] )) 
                                * h;
		Tout+=h;

	}
        void NextStep()
        {
            eps=1;
            double u2 =0;// 2.0 * u / eps_max;
            while ( eps > eps_max )
            {
		
	        tmpVect = CurrVect;
                epsVect = CurrVect;
                PrevVect = CurrVect;
		b[1]=35.0/384.0;
            	b[2]=0;
            	b[3]=500.0/1113.0;
            	b[4]=125.0/192.0;
            	b[5]=-2187.0/6784.0;
           	b[6]=11.0/84.0;
            	b[7]=0;

                k1 = M.getRight( tmpVect, step );
                k2 = M.getRight( tmpVect +  ((k1 * a[2][1])*step), step );
                k3 = M.getRight( tmpVect + (((k1 * a[3][1]) + (k2 * a[3][2]))*step), step );
                k4 = M.getRight( tmpVect + (((k1 * a[4][1]) + (k2 * a[4][2]) + (k3 * a[4][3]))*step), step );
                k5 = M.getRight( tmpVect + (((k1 * a[5][1]) + (k2 * a[5][2]) + (k3 * a[5][3]) + (k4 * a[5][4]))*step), step );
                k6 = M.getRight( tmpVect + (((k1 * a[6][1]) + (k2 * a[6][2]) + (k3 * a[6][3]) + (k4 * a[6][4]) + (k5 * a[6][5])) * step), step );
                k7 = M.getRight( tmpVect + (((k1 * a[7][1]) + (k2 * a[7][2]) + (k3 * a[7][3]) + (k4 * a[7][4]) + (k5 * a[7][5]) + (k6 * a[7][6])) * step), step );
                epsVect  += (( k1 * b1[1] ) +
                             ( k2 * b1[2] ) +
                             ( k3 * b1[3] ) +
                             ( k4 * b1[4] ) +
                             ( k5 * b1[5] ) +
                             ( k6 * b1[6] ) +
                             ( k7 * b1[7] ))
                                * step;
                tmpVect += (( k1 * b[1] ) +
                            ( k2 * b[2] ) +
                            ( k3 * b[3] ) +
                            ( k4 * b[4] ) +
                            ( k5 * b[5] ) +
                            ( k6 * b[6] ) +
                            ( k7 * b[7] )) 
                                * step;

                eps1 = step * ( tmpVect.U  - epsVect.U ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.U ) ),  maxld( abs( PrevVect.U ),  u2 ) ) ;
                eps2 = step * ( tmpVect.dU - epsVect.dU ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.dU ) ),  maxld( abs( PrevVect.dU ),  u2 ) ) ;
              
                eps = sqrt( ( eps1 * eps1 + eps2 * eps2 ) /2.0 );

                step = step / maxld( 0.1, minld( 5.0, pow( eps / eps_max, 0.2) / 0.9 ) );
            }
	t+=step;
	CurrVect = tmpVect;
        }
};

struct DrawPoint
{
    double x,y;
};
class testApp : public ofBaseApp{

	public:
        testApp();
	~testApp()
        {
        }

        Integrator Rung;
        vector<DrawPoint> Result;
		void setup();
		void update();
		void draw();
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


};
