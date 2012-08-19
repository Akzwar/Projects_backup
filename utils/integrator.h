#include "model.h"

template<class DD>
class Integrator
{
 protected:
    DD t, t0, tk, step;
    Vect<DD> CurrVect;
    vector< Vect<DD> > Result;
    Model<DD> *M;
 public:
    Integrator(){}
    Integrator( Model<DD>* _M, DD _t0, DD _tk, DD _step ) 
        : t0(_t0), tk(_tk), step(_step)
    {
        M = _M;
		t = t0;
		CurrVect = M->InitVect();
    }
    virtual void ReinitModel( Model<DD>* _M )
    {
        Result.clear();
        this->M = _M;
        this->t = this->t0;
        CurrVect = M->InitVect();
    }
    virtual void NextStep(){}
    virtual const Vect<DD>& PhaseVect()
    {
        return CurrVect;
    }
    DD T0(){ return t0; }
    DD T(){ return t; }
    DD Tk(){ return tk; }
    DD Step(){ return step; }
};

template<typename DD>
class DPIntegrator : public Integrator<DD>
{
 private:
	DD h, Tout;
	Vect<DD> k[8];
	
	DD a[8][7];
	DD b[8];
	DD sb[8];
	DD b1[8];
	DD c[8];

	DD eps, eps_max, u;
	DD* keps;
	
	Vect<DD> OutVect;
	bool issmallstep;

    Vect<DD> prevVect, tmpVect, epsVect;
	
	void SmallStep()
	{	
		if( this->Tout > this->step )
		{
			BigStep();
			this->Tout=0;
		}
		double teta = this->h/this->step;
		sb[1]=teta*(1+teta*(-1337.0/480.0 + teta*(1039.0/360.0 + teta*(-1163.0/1152.0))));
		sb[2]=0.0;
		sb[3] = 100.0 * teta * teta * (1054.0/9275.0 + teta* ( -4682.0/27825.0 + teta*( 379.0/5565.0 ) ) )/3.0; 
		sb[4] = -5.0*teta*teta*( 27.0/40.0 + teta * (-9.0/5.0 + teta *(83.0/96.0)) )/2.0;
		sb[5] = 18225.0*teta*teta*(-3.0/250.0+teta*(22.0/375.0+teta*(-37.0/600.0)))/848.0;
		sb[6] = -22.0*teta*teta*(-3.0/10.0+teta*(29.0/30.0+teta*(-17.0/24.0)))/7.0;
		OutVect = (( k[1] * sb[1] ) +
                            ( k[2] * sb[2] ) +
                            ( k[3] * sb[3] ) +
                            ( k[4] * sb[4] ) +
                            ( k[5] * sb[5] ) +
                            ( k[6] * sb[6] ) +
                            ( k[7] * sb[7] )) 
                                * h;
       		Tout+=h;
	}

	void BigStep()
	{
		Vect<DD> summ;
		DD s;
		eps = eps_max + 1;
		DD u2 = 2.0 * u / eps_max;
		while ( eps > eps_max )
		{
			tmpVect = this->CurrVect;
			epsVect = this->CurrVect;
			prevVect = this->CurrVect;
		    	
			for( int i = 1; i <= 7; i++ )
			{
				summ = Vect<DD>(tmpVect.size());
				for( int j = 1; j < i; j++ )
				{
					summ += k[j] * a[i][j];
				}
				k[i] = this->M->getRight( tmpVect + ( summ * this->step ), this->step ); 
			}
			summ = Vect<DD>(tmpVect.size());
			for( int i = 1; i <= 7; i++ )
				summ += k[i] * b1[i];
			epsVect += summ * this->step;
			summ = Vect<DD>(tmpVect.size());
			for( int i = 1; i <= 7; i++ )
				summ += k[i] * b[i];
			tmpVect += summ * this->step;
			
			s = 0;
			for( int i = 0; i < this->CurrVect.size(); i++ )
			{
				keps[i] = this->step * ( tmpVect[i] - epsVect[i] )/
						max( max( 1.0e-5, sqrt( tmpVect[i] * tmpVect[i] ) ) , max( sqrt( prevVect[i] * prevVect[i]  ), u2 ) );
				s += keps[i] * keps[i]; 
			}
			eps = sqrt( s / 2.0 );

			this->step = this->step / max( 0.1, min( 5.0, pow( eps / eps_max, 0.2 ) / 0.9 ) );
		}	
	this->t += this->step;
	this->CurrVect = tmpVect;
	}
 public:
	DPIntegrator() : Integrator<DD>(){}
	~DPIntegrator()
	{
		if( keps != NULL )
			delete [] keps;
	}
	DPIntegrator( Model<DD>* _M, DD _t0, DD _tk, DD _eps_max, DD _h = 0 ) 
        : Integrator<DD>( _M, _t0, _tk, 1e-5 ), eps_max(_eps_max), h(_h)
	{
		eps = 0;
		DD v = 1;
		while ( ( 1 + v ) > 1 )
		{
			u = v;
			v = v / 2;
		}
		if( h == 0 )
			issmallstep = false;
		else
		{
			issmallstep = true;
		}
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
		Tout = this->step+1;
		keps = new DD[this->CurrVect.size()];
	}
	
    virtual void ReinitModel(Model<DD>* _M)
    {
        (Integrator<DD>)this->ReinitModel();
        this->step = 1e-5;
        Tout = this->step + 1;
    }


	virtual void NextStep()
	{
		if(issmallstep)
		SmallStep();
		else
		BigStep();
		this->Result.push_back(this->PhaseVect());
	}
	
	virtual Vect<DD> PhaseVect()
	{
		if(issmallstep)
		return OutVect;
		return this->CurrVect;
	}
};

template<typename DD>
class EilerIntegrator : public Integrator<DD>
{
 public:
	EilerIntegrator() : Integrator<DD>(){}
	EilerIntegrator( Model<DD>* _M, DD _t0, DD _tk, DD _step ) : Integrator<DD>( _M, _t0, _tk, _step ){}
	
	virtual void NextStep()
	{
		this->CurrVect += this->M->getRight( this->CurrVect, this->step ) * this->step;
		this->t += this->step;
		this->Result.push_back(this->CurrVect);
	}
};
