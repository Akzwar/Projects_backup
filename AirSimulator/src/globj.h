#include "../../utils/glinclude.h"
#include "../../utils/quat.h"
#include <math.h>

class GLStaticObj3D
{
 protected:
    Vect<double> Pos;
 public:
    GLStaticObj3D( double _X = 0, double _Y = 0, double _Z = 0) 
        : Pos(3,_X,_Y,_Z){}
    virtual ~GLStaticObj3D(){}
    virtual void Draw() = 0;
};

class GLDynamicObj3D : public GLStaticObj3D
{
 protected:
    Vect<double> V;
    Vect<double> A;
    double MaxV, MaxA;
 public:
    GLDynamicObj3D(double _X, double _Y, double _Z, double _MaxV, double _MaxA, 
                                              Vect<double>& _V, Vect<double>& _A ) 
                        : GLStaticObj3D(_X, _Y, _Z), V(_V), A(_A), MaxV(_MaxV), MaxA(_MaxA){}
    virtual void Draw()
    {
        glPointSize(5);
        glBegin(GL_POINTS);
            glVertex3f(this->Pos[V_X],this->Pos[V_Y],this->Pos[V_Z]);
        glEnd();
    }
    virtual void Move( double step )
    {
        this->Pos += ( V + ( A * step ) ) * step;
    }
};

class GLDynamicObj3D_I : public GLDynamicObj3D
{
 protected:
    double MaxdV, MaxdA, MaxdXYAngle, MaxdOZAngle;
 public:
    GLDynamicObj3D_I( double _X, double _Y, double _Z, 
                      double _MaxV, double _MaxA, double _MaxdV, double _MaxdA,
                      double _MaxdXYAngle, double _MaxdOZAngle,
                      Vect<double>& _V, Vect<double>& _A )
        :GLDynamicObj3D( _X, _Y, _Z, _MaxV, _MaxA, _V, _A ), MaxdV(_MaxdV), MaxdA(_MaxdA),
         MaxdXYAngle(_MaxdXYAngle), MaxdOZAngle(_MaxdOZAngle){}
};

class GLDynamicObj3D_AI : public GLDynamicObj3D_I
{
 public:
    GLDynamicObj3D_AI( double _X, double _Y, double _Z, 
                      double _MaxV, double _MaxA, double _MaxdV, double _MaxdA,
                      double _MaxdXYAngle, double _MaxdOZAngle,
                      Vect<double>& _V, Vect<double>& _A )
        :GLDynamicObj3D_I( _X, _Y, _Z, _MaxV, _MaxA, _MaxdV, _MaxdA, _MaxdXYAngle, _MaxdOZAngle,
                      _V, _A ){}
   virtual void Load() = 0;
};

class GLDynamicObj3D_HI : public GLDynamicObj3D_I
{
 public:
    //virtual void IncreaseSpeed( double dSpeed, double step ){V += ( dSpeed < MaxdV )? dSpeed * step : MaxdV * step;}
    //virtual void DecreaseSpeed( double dSpeed, double step ){V -= ( dSpeed < MaxdV )? dSpeed * step : MaxdV * step;}//TODO IncreaseMaxSpeed != DecreaseMaxSpeed
    GLDynamicObj3D_HI( double _X, double _Y, double _Z, 
                      double _MaxV, double _MaxA, double _MaxdV, double _MaxdA,
                      double _MaxdXYAngle, double _MaxdOZAngle,
                      Vect<double>& _V, Vect<double>& _A )
        :GLDynamicObj3D_I( _X, _Y, _Z, _MaxV, _MaxA, _MaxdV, _MaxdA, _MaxdXYAngle, _MaxdOZAngle,
                      _V, _A ){}

    virtual void IncreaseAccel( double dAccel, double step )
    {
        if( A.Length() == 0 )
            A = ( dAccel < MaxdA )? V / V.Length() * dAccel * step : V / V.Length() * MaxdA * step ;
        else
            A = ( dAccel < MaxdA )? A / A.Length() * ( A.Length() + dAccel * step ) : A / A.Length() * ( A.Length() + MaxdA * step ) ;
    }
    virtual void DecreaseAccel( double dAccel, double step )
    {
        if( A.Length() == 0 )
            A = ( dAccel < MaxdA )? V / V.Length() * dAccel * ( -step ) : V / V.Length() * MaxdA * ( -step ) ;
        else
            A = ( dAccel < MaxdA )? A / A.Length() * ( A.Length() - dAccel * step ) : A / A.Length() * ( A.Length() - MaxdA * step ) ;
    }
    virtual void TurnRight( double Angle, double step )
    {
        if( Angle < MaxdXYAngle )
            V = Rotate( V, -Angle * step, Vect<double>(3,0.0,0.0,1.0) );
        else
            V = Rotate( V, -MaxdXYAngle * step, Vect<double>(3,0.0,0.0,1.0) );
    }
    virtual void TurnLeft( double Angle, double step )
    {
        if( Angle < MaxdXYAngle )
            V = Rotate( V, Angle * step, Vect<double>(3,0.0,0.0,1.0) );
        else
            V = Rotate( V, MaxdXYAngle * step, Vect<double>(3,0.0,0.0,1.0) );
    }
    virtual void TurnUp( double Angle, double step )//TODO Check if Down
    {
        if( Angle < MaxdXYAngle )
            V = Rotate( V, Angle * step, Vect<double>(3,0.0,0.0,1.0) % V );
        else
            V = Rotate( V, MaxdXYAngle * step, Vect<double>(3,0.0,0.0,1.0) % V );
    }
    virtual void TurnDown( double Angle, double step )//TODO Check if Up
    {
        if( Angle < MaxdXYAngle )
            V = Rotate( V, -Angle * step, Vect<double>(3,0.0,0.0,1.0) % V );
        else
            V = Rotate( V, -MaxdXYAngle * step, Vect<double>(3,0.0,0.0,1.0) % V );
    }
};
