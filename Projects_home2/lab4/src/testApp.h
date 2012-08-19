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

        Vect(){}

        Vect(const Vect &V)
        {
		for( int i = 0; i<V.size(); i++)
			this->data.push_back(V.data[i]);
        }
	vector<double> data;
        Vect(long double *n, int length)
        {
		for( int i = 0; i < length; i++ )
			data.push_back(n[i]);
        }

        Vect operator + (const Vect& V) const
        {
            	Vect retVect;
		for( int i = 0; i < this->size(); i++)
                	retVect.data.pushback(V.data[i]+this->data[i]);
		return retVect;
        }

        void operator += (const Vect& V)
        {	    
		for( int i = 0; i < this->size(); i++ )
			this->data[i]+=V.data[i];
        }

        Vect operator * (const long double Cross) const
        {
            	Vect retVect;
	    	for( int i = 0; i < this->size(); i++)
			retVect.data.push_back( this->data[i] * Cross );
            	return retVect;
        }

        bool operator == (const Vect& V) const
        {
		if( V.size() != this->size())
			return false;
		for( int i = 0; i < this; i++ )
			if( this->data[i] != V.data[i])
				return false;
		return true;
        }

        bool operator != (const Vect& V) const
        {
            return !((*this)==V);
        }

        inline void operator = (const Vect& V)
        {
		for( int i = 0; i < this->size(); i++ )
			this->data[i] = V.data[i];
        }

        //double Angle()const {return atan2(Y2, long double Y1);}
};

class CourseWorkModel
{
    private:
        long double _Vce, _Vbe, _Il, _I1, _dI1, _IL01, _dIL01, _I2, _dI2, _IL02, _dIL02, _I3, _dI3, _IL03, _dIL03, ddIL01, ddI1, ddIL02, ddI2, ddIL03, ddI3, dVce, dVbe, dIl;
	double Vc, Ve, Rl, Re, Ic, I0, Vt, Ib, beta, Lg, L, L0, C0, C1, C2, C, R, R0;
	
    public:
        Model()
        {
            InitVect = Vect(0.994, 0, 0, -2.0317326295573368357302057924,0,0,0,0,0,0,0,0,0,0,0);
	    Vc=7.0;
	    Ve=1.6;
	    Rl=40.0;
	    Re=400.0;
	    Vt=26.0;
	    beta=100.0;
	    Ib=(26.0/5.0)/beta;
	    Lg=L=L0=0.5;
	    C1=C2=C=C0=1.0;
	    R=R0=5.0;
        }
        Vect InitVect;
        Vect getRight(const Vect& V )
        {
            Vect retVect;
	    /*_Vce = V.Vce;
	    _Vbe = V.Vbe;
	    _Il = V.Il;
	    _I1 = V.I1;
	    _dI1 = V.dI1;
	    _IL01 = V.IL01;
	    _dIL01 = V.dIL01;
	    _I2 = V.I2;
	    _dI2 = V.dI2;
	    _IL02 = V.IL02;
	    _dIL02 = V.dIL02;
	    _I3 = V.I3;
	    _dI3 = V.dI3;
	    _IL03 = V.IL03;
	    _dIL03 = V.dIL03;
	    
	    Ic = I0*exp(_Vbe/Vt-1);
	    
	    dVce = (_Il-_I1)/C1-Ic;
	    dVbe = ((Ve-_Vbe)/Re-_Il-Ib)/C2;
	    dIl = (Vc-_Vce-Rl*_Il-_Vbe)/Lg;
	    ddI1 = ((_Il-Ic)/C1+(_I2-_IL01)/C0-R*_dI1-(1.0/C + 1/C0 + 1.0/C1)*_I1)/L;
	    ddIL01 = ((_I1-_I2)/C0-R0*_dIL01 -1/C0*_IL01)/L0;
	    ddI2 = ((_I1+_I3+_IL02-_IL01)/C0-R*_dI2-(1.0/C+2.0/C0)*_I2)/L;
	    ddIL02 = ((_I2-_I3)/C0-R0*_dIL02+1/C0*_IL02)/L0;
	    ddI3 = ((_I2+_IL03-_IL02)/C0-R*_dI3-(1.0/C+2.0/C0)*_I3)/L;
	    ddIL03 = ((_I3)/C0-R0*_dIL03+1/C0*_IL03)/L0;
	    
            retVect.Vbe = dVbe;
	    retVect.Vce = dVce;
	    retVect.Il = dIl;
	    retVect.I1 = _dI1;
	    retVect.dI1 = ddI1;
	    retVect.I2 = _dI2;
	    retVect.dI2 = ddI2;
	    retVect.I3 = _dI3;
	    retVect.dI3 = ddI3;
	    retVect.IL01 = _dIL01;
	    retVect.dIL01 = ddIL01;
	    retVect.IL02 = _dIL02;
	    retVect.dIL02 = ddIL02;
	    retVect.IL03 = _dIL03;
	    retVect.dIL03 = ddIL03;*/
	    long double nu = 0.012277471;
            long double ns = 1 - nu;
	    long double y1  = V.Vce;
            long double y2  = V.Vbe;
            long double yd1 = V.Il;
            long double yd2 = V.I1;

            long double y1nu = y1 + nu;
            long double y1ns = y1 - ns;

            long double pd1 = y1nu * y1nu + y2 * y2;
           long double  pd2 = y1ns * y1ns + y2 * y2;

            long double D1 = pd1 * sqrt(pd1);
            long double D2 = pd2 * sqrt(pd2);

            long double ydd1 =  y1 + 2 * yd2 - ns * y1nu / D1 - nu * y1ns / D2;
            long double ydd2 =  y2 - 2 * yd1 - ns * y2   / D1 - nu * y2   / D2;

            retVect.Vce  = yd1;
            retVect.Vbe = yd2;
            retVect.Il = ydd1;
            retVect.I1 = ydd2;


            return retVect;
        }
};
class Integrator
{
    private:
        long double t;
        Vect k1, k2, k3, k4, k5, k6, k7;


        long double a[8][7];
        long double b[7];
        long double b1[7];
        long double c[7];

        long double eps, eps_max, u, eps1, 
				eps2, 
				eps3, 
				eps4,
				eps5,
				eps6,
				eps7,
				eps8,
				eps9,
				eps10,
				eps11,
				eps12,
				eps13,
				eps14,
				eps15;

        Vect PrevVect;
        Vect tmpVect;
        Vect epsVect;
    public:
        Integrator(){}

        Integrator(Model& _M, long double _t0, long double _tk, long double _step)
        {
            eps=0;
            eps_max = 0.0005;
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
        }
        Model M;
        Vect CurrVect;
        long double getT(){return t;}
        long double getT0(){return t0;}
        long double getTk(){return tk;}
        long double getStep(){return step;}
        long double t0;
        long double tk;
        long double step;

        void NextStep()
        {
            eps=1;
            double u2 = 0;
            while ( eps > eps_max )
            {
                tmpVect = CurrVect;
                epsVect = CurrVect;
                PrevVect = CurrVect;

                k1 = M.getRight( tmpVect );
                k2 = M.getRight( tmpVect +  ((k1 * a[2][1])*step));
                k3 = M.getRight( tmpVect + (((k1 * a[3][1]) + (k2 * a[3][2]))*step));
                k4 = M.getRight( tmpVect + (((k1 * a[4][1]) + (k2 * a[4][2]) + (k3 * a[4][3]))*step));
                k5 = M.getRight( tmpVect + (((k1 * a[5][1]) + (k2 * a[5][2]) + (k3 * a[5][3]) + (k4 * a[5][4]))*step));
                k6 = M.getRight( tmpVect + (((k1 * a[6][1]) + (k2 * a[6][2]) + (k3 * a[6][3]) + (k4 * a[6][4]) + (k5 * a[6][5])) * step));
                k7 = M.getRight( tmpVect + (((k1 * a[7][1]) + (k2 * a[7][2]) + (k3 * a[7][3]) + (k4 * a[7][4]) + (k5 * a[7][5]) + (k6 * a[7][6])) * step));
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

                eps1 = step * ( tmpVect. Vce - epsVect. Vce ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect. Vce ) ),  maxld( abs( PrevVect. Vce ),  u2 ) ) ;
                eps2 = step * ( tmpVect. Vbe - epsVect. Vbe ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect. Vbe ) ),  maxld( abs( PrevVect. Vbe ),  u2 ) ) ;
                eps3 = step * ( tmpVect.Il - epsVect.Il ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.Il ) ),  maxld( abs( PrevVect.Il ),  u2 ) ) ;
                eps4 = step * ( tmpVect.I1 - epsVect.I1 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.I1 ) ),  maxld( abs( PrevVect.I1 ),  u2 ) ) ;
                /*eps5 = step * ( tmpVect.dI1 - epsVect.dI1 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.dI1 ) ),  maxld( abs( PrevVect.dI1 ),  u2 ) ) ;
                eps6 = step * ( tmpVect.IL01 - epsVect.IL01 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.IL01 ) ),  maxld( abs( PrevVect.IL01 ),  u2 ) ) ;
                eps7 = step * ( tmpVect.dIL01 - epsVect.dIL01 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.dIL01 ) ),  maxld( abs( PrevVect.dIL01 ),  u2 ) ) ;
                eps8 = step * ( tmpVect.I2 - epsVect.I2 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.I2 ) ),  maxld( abs( PrevVect.I2 ),  u2 ) ) ;
                eps9 = step * ( tmpVect.dI2 - epsVect.dI2 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.dI2 ) ),  maxld( abs( PrevVect.dI2 ),  u2 ) ) ;
                eps10 = step * ( tmpVect.IL02 - epsVect.IL02 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.IL02 ) ),  maxld( abs( PrevVect.IL02 ),  u2 ) ) ;
                eps11 = step * ( tmpVect.dIL02 - epsVect.dIL02 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.dIL02 ) ),  maxld( abs( PrevVect.dIL02 ),  u2 ) ) ;
                eps12 = step * ( tmpVect.I3 - epsVect.I3 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.I3 ) ),  maxld( abs( PrevVect.I3 ),  u2 ) ) ;
                eps13 = step * ( tmpVect.dI3 - epsVect.dI3 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.dI3 ) ),  maxld( abs( PrevVect.dI3 ),  u2 ) ) ;
                eps14 = step * ( tmpVect.IL03 - epsVect.IL03 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.IL03 ) ),  maxld( abs( PrevVect.IL03 ),  u2 ) ) ;
                eps15 = step * ( tmpVect.dIL03 - epsVect.dIL03 ) /
                       maxld( maxld( 1.0e-5,  abs( tmpVect.dIL03 ) ),  maxld( abs( PrevVect.dIL03 ),  u2 ) ) ;*/
              
                eps = sqrt( ( eps1 * eps1 + eps2 * eps2 + eps3 * eps3 + eps4 * eps4 )*0.25);//+ eps5 * eps5 + eps6 * eps6 + eps7 * eps7 + eps8 * eps8 + eps9 * eps9 + eps10 * eps10 + eps11 * eps11 + eps12 * eps12 + eps13 * eps13 + eps14 * eps14 + eps15 * eps15  ) /15.0 );
                if(eps<=eps_max){
                    t += step;
                }else{
                    step = step / maxld( 0.1, minld( 5.0, pow( eps / eps_max, 0.2) / 0.9 ) );
                }
            }
            CurrVect = tmpVect;
        }
};

struct DrawPoint
{
    double x, y;
};
class testApp : public ofBaseApp{

	public:
        testApp();
        ~testApp()
        {
        }

        Integrator Rung;
        //vector<DrawPoint> Result;
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