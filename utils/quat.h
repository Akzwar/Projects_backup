#include "matrix.h"

template<typename DD>
class Quat
{
 public:
	DD a;
	Vect<DD> V;

	Quat(){}

	Quat( const Quat& Q )
	{
		this->a = Q.a;
		this->V = Q.V;
	}

	Quat( long double _a, const Vect<DD>& _V )
	{
		this->a = _a;
		this->V = _V;
	}

	Quat operator + ( const Quat& Q )
	{
		Quat<DD> retQuat( this->a + Q.a, this->V + Q.V );
		return retQuat;
	}

	Quat operator * ( const Quat& Q )
	{
		Quat<DD> retQuat( (this->a * Q.a) - (this->V * Q.V), ( Q.V * this->a ) + ( this->V * Q.a ) + ( this->V % Q.V ) );
		return retQuat;
	}
			
};

Vect<double> Rotate( const Vect<double>& V, double Angle, const Vect<double>& Axis )
{
    Quat<double> Q( cos( Angle / 2.0 ), Axis / Axis.Length() * sin( Angle / 2.0 ) );
    Quat<double> Qm( cos( Angle / 2.0 ), Axis / Axis.Length() * -sin( Angle / 2.0 ) );
    Vect<double> retVect = (Q * Quat<double>( 0, V ) * Qm).V;
    return retVect;
}
