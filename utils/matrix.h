#include "vect.h"

using namespace std;

template<typename DD>
class Matrix
{
 private:
	vector< Vect<DD> > data;
 public:
	Matrix(){};
	Matrix( int n, int m )
	{
		Vect<DD> Row(n);
		for( int i = 0; i < m; i++ )
			data.push_back(Row);
	}
	
	Vect<DD>& operator [] ( int index )
	{
		return data[index];
	}
	
	int getWidth() const
	{
		return this->data[0].size();
	}
	
	int getHeight() const
	{
		return this->data.size();
	}

	void Transpose()
	{
		vector< Vect<DD> > resdata( this->getWidth(), Vect<DD> ( this->getHeight() ) );
		for( int i = 0; i < this->getWidth(); i++ )
			for( int j = 0; j < this->getHeight(); j++ )
			resdata[i][j] = this->data[j][i];
		this->data = resdata; 
	}
	
	const Matrix operator * ( DD Cross ) 
	{
		Matrix<DD> resMatrix( this->getWidth(), this->getHeight() );
		for( int i = 0; i < this->getWidth(); i++ )
			for( int j = 0;	j < this->getHeight(); j++ )
			resMatrix[i][j] = this->data[i][j] * Cross;
		return resMatrix;
	}
	
	const Matrix operator + ( Matrix& M )
	{	
		Matrix<DD> resMatrix( this->getWidth(), this->getHeight() );
		for( int i = 0; i < this->getWidth(); i++ )
			for( int j = 0;	j < this->getHeight(); j++ )
			resMatrix[i][j] = this->data[i][j] + M.data[i][j];	
		return resMatrix;	
	}

	const Matrix operator * ( Matrix& M )
	{	
		Matrix<DD> resMatrix( M.getWidth(), this->getHeight());
		for( int i = 0; i < this->getHeight(); i++ )
			for( int j = 0;	j < M.getWidth(); j++ )
				for( int k = 0; k < this->getWidth(); k++ )
					resMatrix[i][j] += this->data[i][k] * M.data[k][j];
		return resMatrix;
	}
	
	void print()
	{
		for( int i = 0; i < this->getHeight(); i++ )
		{
			printf("\n");
			for( int j = 0; j < this->getWidth(); j++ )
				printf("[%f]",(double)this->data[i][j]);
		}
		printf("\n");
	}
	
	void addRow( Vect<DD>& V )
	{
		this->data.push_back(V);
	}	
};
