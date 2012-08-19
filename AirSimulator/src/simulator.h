#include "globj.h"
#include <vector>

using namespace std;

class Simulator
{
 private:
    double t0, tk, step, t;
    GLDynamicObj3D_HI* MainObj;
    vector< GLDynamicObj3D_AI* > Objects;
 public:
    Simulator(double InitX, double InitY, double InitZ, double _t0, double _tk, double _step)
        :t0(_t0), tk(_tk), step(_step), t(0)
    {
        MainObj = new GLDynamicObj3D_HI( InitX, InitY, InitZ, 150.0, 10.0, 1.0, 1.0, 0.4, 0.4, 
                                        Vect<double>( 3, 50.0,0.0,0.0 ), Vect<double>( 3, 0.0,0.0,0.0 ) )
    }
    ~Simulator()
    {
        for( int i = 0; i < Object.size(); i++ )
            delete Objects[i];
        if( MainObj )
            delete MainObj;
    }
    void AddObj( double X, double Y, double Z, double MaxV, double MaxA, double MaxdV,
                    double MaxdA, double MaxdXYAngle, double MaxdOZAngle,
                    Vect<double>& V, Vect<double>& A)
    {
        Objects.push_back( new GLDynamicObj3D_AI( X, Y, Z, MaxV, MaxA, MaxdV, MaxdA, 
                                                    MaxdXYAngle, MaxdOZAngle, V, A) );
    }

    void NextStep()
    {
        t += step; 
        MainObj->Move(step);
        for( int i = 0; i < Objects.size(); i++ ) 
            Objects[i]->Move(step);
    }
    void Draw()
    {
        MainObj->Draw();
        for( int i = 0; i < Objects.size(); i++ ) 
            Objects[i]->Draw();
    }
}
