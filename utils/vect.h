#include <vector>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#define V_X 0
#define V_Y 1
#define V_Z 2
#define V_dX 3
#define V_dY 4
#define V_dZ 5

using namespace std;

template<typename DD>
class Vect
{
    private:
	
	vector< DD > data;	

    public:

        Vect(){}
	
	int size() const
	{
		return data.size();
	}	
	Vect( int length )
	{
		for( int i = 0; i < length; i++ )
			this->data.push_back(0);
	
	}
        Vect(const Vect &V)
        {
		this->data.clear();
		for( int i = 0; i<V.size(); i++)
			this->data.push_back(V.data[i]);
        }
        Vect(DD *n, int length)
        {
		this->data.clear();
		for( int i = 0; i < length; i++ )
			data.push_back(n[i]);
        }
        int ln;
    Vect( int s, DD X, ... )
    {
        ln++;
        //printf("%d\n",ln);
        va_list l;
        va_start( l, X );
        this->data.push_back(X);
        for( int i = 0; i < s - 1; i++ )
            this->data.push_back(va_arg(l,DD));
        va_end(l);
    }
	Vect( DD X, DD Y, DD Z )
	{
		this->data.push_back(X);
		this->data.push_back(Y);
		this->data.push_back(Z);
	}
  
    Vect operator + (const Vect& V) const
        {
            	Vect retVect;
		for( int i = 0; i < this->size(); i++)
                	retVect.data.push_back(V.data[i]+this->data[i]);
		return retVect;
        }

        void operator += (const Vect& V)
        {	    
		for( int i = 0; i < this->size(); i++ )
			this->data[i]+=V.data[i];
        }

    Vect operator -= ( const Vect & V )
    {	
        this->print();
        V.print();
        for( int i = 0; i < this->size(); i++ )
			this->data[i]-=V.data[i];
    }

	const Vect operator * ( DD Cross ) const
        {
            	Vect retVect;
	    	for( int i = 0; i < this->size(); i++)
			retVect.data.push_back( this->data[i] * Cross );
            	return retVect;
        }

	DD operator * ( const Vect& V ) const
	{
		DD ret = 0;
		for( int i = 0; i < this->size(); i++ )
			ret += this->data[i] * V.data[i] ;
		return ret;
	}

	Vect operator % ( const Vect& V )
	{
		Vect retVect(3);
		retVect[V_X] = this->data[V_Y] * V.data[V_Z] - this->data[V_Z] * V.data[V_Y];
		retVect[V_Y] = this->data[V_Z] * V.data[V_X] - this->data[V_X] * V.data[V_Z];
		retVect[V_Z] = this->data[V_X] * V.data[V_Y] - this->data[V_Y] * V.data[V_X];
		return retVect;
	}

    Vect operator / ( DD Value ) const
    {
        Vect retVect = (*this) * ( 1 / (double)Value );
        return retVect;
    }
        bool operator == (const Vect& V) const
        {
		if( V.size() != this->size())
			return false;
		for( int i = 0; i < this->size(); i++ )
			if( this->data[i] != V.data[i])
				return false;
		return true;
        }

        bool operator != (const Vect& V) const
        {
            return !((*this)==V);
        }

        void operator = (const Vect& V)
        {
		this->data.clear();
		for( int i = 0; i < V.size(); i++ )
			this->data.push_back( V.data[i] );
        }
	
 	DD& operator [] ( int index )
	{
		return data[index];
	}
	
	void SetElement( int index, DD value)
	{
		data[index] = value;
	}
	
	void print()
	{
		for(int i = 0; i < this->size(); i++)
			if( i != this->size() - 1 )
				printf("[%f],", (double)this->data[i]);
			else 
				printf("[%f]\n", (double)this->data[i]);
	}
	
	DD Length() const
	{
		DD summ=0;
		for(int i=0;i<3;i++)
			summ+=data[i]*data[i];
		return sqrt( summ );
	}
};

